
#include "image.h"
#include "chunk.h"
#include "filter.h"


/**
 * @brief Number of sample of a pixel 
 * @param[in] type Color type
 * @return Count of sample
 */
static uint8_t count_sample(enum color_type type) {
  switch (type) {
  case PLTE_INDEX:
  case GRAYSCALE:
    return 1;
  case GRAYSCALE_ALPHA:
    return 2;
  case RGB_TRIPLE:
    return 3;
  case RGB_TRIPLE_ALPHA:
    return 4;
  }
}

/**
 * @brief Compute the length of a scanline
 * @param[in] depth
 * @param[in] sample Count of sample
 * @param[in] width
 * @return Number of byte for a pixel (or index) line in the image
 */
static uint32_t byte_per_line(uint8_t depth, uint8_t sample, uint32_t width) {

  uint32_t bit_per_line = depth * sample * width;
  return (bit_per_line + 7) / 8; // round up to one if % 8 != 0
}

/**
 * @brief Compute the needed size for the decompression of IDAt chunk
 * @details Add one [filter-type byte](http://www.libpng.org/pub/png/spec/1.2/PNG-DataRep.html#DR.Filtering)
 * to each scanline to get the total size
 * @param[in] header
 * @return size
 */
static uint32_t unpack_size(const struct IHDR *header) {
  return (1 + byte_per_line(header->depth, count_sample(header->color_type), header->width)) * header->height;
}

/**
 * @brief Consume all IDAT chunk to inflate all image data (using zlib only in this function)
 * @param[in] file_size
 * @param[in] file_ptr Allocated file content (pointer to the begining of a chunk before IDAT)
 * @param[in] img_size Size allocated starting at img_ptr
 * @param[out] img_ptr Pointer to the area to fill with unpack data
 * @details IDAT chunk must be [consecutive](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.Summary-of-standard-chunks)
 */
static void unpack_data(uint32_t file_size, const uint8_t *file_ptr, uint32_t img_size, void *img_ptr) {

  // find the first IDAT chunk of the file
  struct chunk current = get_chunk(file_size, file_ptr);
  while (current.type != IDAT) {
    file_size -= (current.length + 12); // chunk size
    file_ptr  += (current.length + 12);
    current = get_chunk(file_size, file_ptr);
  }
  assert(current.type == IDAT);

  // init z_stream value
  z_stream stream;
  stream.zalloc = Z_NULL;
  stream.zfree  = Z_NULL;
  stream.opaque = (voidpf) 0;
  stream.next_in   = (z_const Bytef *) current.data; // drop the const but it's ok (z_const)
  stream.avail_in  = current.length;
  stream.next_out  = img_ptr;
  stream.avail_out = img_size;
  LOG_INFO("Inflate IDAT ...");
  
  // inflate init
  int err = inflateInit(&stream);
  if (err != Z_OK) {
    LOG_FATAL("InflateInit failed, returned %d", err);
    exit(1);
  }

  // loop through each IDAT
  while (current.type == IDAT) {

    while (stream.avail_in > 0) {
      err = inflate(&stream, Z_NO_FLUSH); // consume data in the chunk
      
      if (err == Z_STREAM_END) {
        break; // zlib know it is the last IDAT 
      }
      if (err != Z_OK) {
        LOG_FATAL("Inflate failed, returned %d", err);
        exit(1);
      }
    }
    
    // get the next IDAT to consume
    file_size -= (current.length + 12);
    file_ptr  += (current.length + 12);
    current = get_chunk(file_size, file_ptr);

    if ((err == Z_STREAM_END) && (current.type == IDAT)) {
      LOG_WARN("Weird, zlib said it inflates the whole image but there is still IDAT left");
      break;
    }
    stream.next_in  = (z_const Bytef *) current.data;
    stream.avail_in = current.length;
  }
  if (stream.avail_out != 0) {
    LOG_FATAL("Inflating IDAT didn't take as much space as expected, remaind %d byte", stream.avail_out);
    exit(1);
  }

  // end inflate
  err = inflateEnd(&stream);
  if (err != Z_OK) {
    LOG_FATAL("InflateEnd failed, returned %d", err);
    exit(1);
  }
  LOG_INFO("Inflate IDAT done");
}



const struct image image_from_png(const struct mfile *file) {
  assert(mfile_is_png(file) == 1);

  // skip the signature and get header
  uint32_t file_size = file->size - 8;
  uint8_t *file_ptr  = ((uint8_t *) file->data) + 8;
  // get the header chunk
  const struct chunk current = get_chunk(file_size, file_ptr);
  const struct IHDR header   = IHDR_chunk(&current);

  // limitation
  if (header.color_type == PLTE_INDEX) {
    LOG_FATAL("Color type (PLTE) not handle YET");
    exit(0);
  }
  if (header.interlace == 1) {
    LOG_FATAL("Interlacing methode (ADAM7) no handle YET");
    exit(0);
  }

  // compute the size of the unpack image
  uint32_t img_size = unpack_size(&header);
  void    *img_data = malloc(img_size);
  if (img_data == NULL) {
    LOG_FATAL("Can't allocate %d byte to get the image", img_size);
    exit(1);
  }
  LOG_DEBUG("Malloc(%d) from %p", img_size, img_data);

  // skip the header chunk
  file_size -= (current.length + 12);
  file_ptr  += (current.length + 12);
  // inflate image
  unpack_data(file_size, file_ptr, img_size, img_data);

  struct image img = {
    .width   = header.width,
    .height  = header.height,
    .depth   = header.depth,
    .sample  = count_sample(header.color_type),
    .palette = NULL,
    .data    = img_data,
  };

  // unfilter
  unfilter(&img);

  return img;
}



uint32_t line_size(const struct image *image) {
  return byte_per_line(image->depth, image->sample, image->width);
}



void free_image(const struct image *image) {
  free(image->data);
  LOG_DEBUG("Free image");
}




/**
 * @brief Compute the pointer to the pixel (i, j)
 * @details A sample hold on at least one byte, which make easier to compute the pointer
 * @param[in] image
 * @param[in] i
 * @param[in] j
 * @return the pointer
 */
static void *pixel_pointer(const struct image *image, uint32_t i, uint32_t j) {
  assert((image->depth == 8) || (image->depth == 16));

  uint8_t pixel_size = (image->depth / 8) * image->sample;
  uint32_t line_size = pixel_size * image->width;

  return image->data + (i * (line_size + 1)) + 1 + (pixel_size * j);
}

/**
 * @brief Compute the pointer to the byte holding the pixel (i, j)
 * @details A sample is less than a byte, so the pixel is in the bit, left_shift bits from the left 
 * @param[in] image
 * @param[in] i
 * @param[in] j
 * @param[out] left_shift
 * @return the pointer
 */
static uint8_t *sample_pointer(const struct image *image, uint32_t i, uint32_t j, uint8_t *left_shift) {
  assert(image->depth != 16);

  uint32_t line_size = byte_per_line(image->depth, image->sample, image->width);
  uint32_t bit_shift = image->depth * image->sample * j;

  *left_shift = bit_shift % 8;
  return image->data + (i * (line_size + 1)) + 1 + (bit_shift / 8);
}

/**
 * @brief Extract the sample from one byte
 * @param[in] byte The byte where the sample is
 * @param[in] shift Shift in bits from the left to get the sample (0 <= shift < 8)
 * @param[in] depth Size of the sample in bits (shift + depth <= 8)
 * @param[out] max Max value of the sample
 * @return The sample value on a byte
 */
static uint8_t get_sample(uint8_t byte, uint8_t left_shift, uint8_t depth, uint8_t *max) {
  // set the last depth bits to 1
  uint8_t mask = (((uint16_t) 1) << depth) - 1;
  // shift from the right to get the sample
  uint8_t right_shift = 8 - left_shift - depth;

  *max = mask;
  return (byte >> right_shift) & mask;
}


void get_color(const struct image *image, uint32_t i, uint32_t j, struct color *color) {
  assert((0 <= i) && (i < image->height));
  assert((0 <= j) && (j < image->width));

  if (image->palette != NULL) { // PLTE: depth 1 2 4 8
    uint8_t max;
    uint8_t left_shift;
    uint8_t *ptr = sample_pointer(image, i, j, &left_shift);
    uint8_t index = get_sample(*ptr, left_shift, image->depth, &max);

    uint8_t *pixel = image->palette + (index * 3);

    color->red   = pixel[0];
    color->green = pixel[1];
    color->blue  = pixel[2];
    color->alpha = max;
    color->max   = max;
    return;
  }

  switch (image->sample) {

  case 1: // Grayscale
    if (image->depth < 8) { // depth 1 2 4
      uint8_t max;
      uint8_t left_shift;
      uint8_t *ptr = sample_pointer(image, i, j, &left_shift);
      uint8_t gray = get_sample(*ptr, left_shift, image->depth, &max);

      color->red   = gray;
      color->green = gray;
      color->blue  = gray;
      color->alpha = max;
      color->max   = max;
    } else if (image->depth == 8) { // depth 8
      uint8_t * ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[0];
      color->blue  = ptr[0];
      color->alpha = 0xff;
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      uint16_t gray = ntohs(ptr[0]);
      color->red   = gray;
      color->green = gray;
      color->blue  = gray;
      color->alpha = 0xffff;
      color->max   = 0xffff;
    }
    break;
  case 2: // Grayscale + Alpha
    if (image->depth == 8) { // depth 8
      uint8_t *ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[0];
      color->blue  = ptr[0];
      color->alpha = ptr[1];
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      uint16_t gray = ntohs(ptr[0]);
      color->red   = gray;
      color->green = gray;
      color->blue  = gray;
      color->alpha = ntohs(ptr[1]);
      color->max   = 0xffff;
    }
    break;
  case 3: // RGB
    if (image->depth == 8) { // depth 8
      uint8_t *ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[1];
      color->blue  = ptr[2];
      color->alpha = 0xff;
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      color->red   = ntohs(ptr[0]);
      color->green = ntohs(ptr[1]);
      color->blue  = ntohs(ptr[2]);
      color->alpha = 0xffff;
      color->max   = 0xffff;
    }
    break;
  case 4: // RGB + Alpha
    if (image->depth == 8) { // depth 8
      uint8_t *ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[1];
      color->blue  = ptr[2];
      color->alpha = ptr[3];
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      color->red   = ntohs(ptr[0]);
      color->green = ntohs(ptr[1]);
      color->blue  = ntohs(ptr[2]);
      color->alpha = ntohs(ptr[3]);
      color->max   = 0xffff;
    }
    break;
  default:
    assert(0);
  }
}
