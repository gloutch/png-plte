
#include "image.h"


/**
 * @brief Compute the needed size for the decompression of IDAt chunk
 * @param[in] width Width of the image
 * @param[in] height Height of the image
 * @param[in] depth Number of bits per sample of palette index
 * @param[in] type Color type of the image
 * @return size
 */
static uint32_t compute_image_size(uint32_t width, uint32_t height, uint8_t depth, enum color_type type) {

  uint32_t bit_per_pixel = 0;
  switch (type) {
  case GRAYSCALE:
  case PLTE_INDEX:
    bit_per_pixel = depth;
    break;
  case GRAYSCALE_ALPHA:
    bit_per_pixel = depth * 2;
    break;
  case RGB_TRIPLE:
    bit_per_pixel = depth * 3;
    break;
  case RGB_TRIPLE_ALPHA:
    bit_per_pixel = depth * 4;
    break;
  }

  uint32_t byte_per_line = 0;
  if ((bit_per_pixel * width) % 8 == 0) { // no bit wasted
    byte_per_line = (bit_per_pixel * width) / 8;  
  }
  else { // add one last byte
    LOG_INFO("The last %d bits are wasted on each scanline", (bit_per_pixel * width) % 8);
    byte_per_line = ((bit_per_pixel * width) / 8) + 1;
  }
  byte_per_line += 1; // filter-type byte to each scanline

  return byte_per_line * height;
}

/**
 * @brief Consume all IDAT chunk to inflate all image data (using zlib only in this function)
 * @param[in] file_size
 * @param[in] file_ptr Allocated file content (pointer to the begining of a chunk before IDAT)
 * @param[in] img_size Size allocated starting at img_ptr
 * @param[out] img_ptr Pointer to the area to fill with unpack data
 * @details IDAT chunk must be [consecutive](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.Summary-of-standard-chunks)
 */
static void unpack_IDAT(uint32_t file_size, const uint8_t *file_ptr, uint32_t img_size, void *img_ptr) {

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

  // inflate end
  err = inflateEnd(&stream);
  if (err != Z_OK) {
    LOG_FATAL("InflateEnd failed, returned %d", err);
    exit(1);
  }
  LOG_INFO("Inflate IDAT done");
}



const struct image image_from_file(const struct mfile *file) {
  assert(mfile_is_png(file) == 1);

  // skip the signature and get header
  uint32_t file_size = file->size - 8;
  uint8_t *file_ptr  = ((uint8_t *) file->data) + 8;
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

  // malloc the right size
  uint32_t img_size = compute_image_size(header.width, header.height, header.depth, header.color_type);
  void *img_data = malloc(img_size);
  if (img_data == NULL) {
    LOG_FATAL("Can't allocate %d byte to get the image", img_size);
    exit(1);
  }
  LOG_DEBUG("Malloc(%d) from %p", img_size, img_data);

  // inflate image
  file_size -= (current.length + 12);
  file_ptr  += (current.length + 12);
  unpack_IDAT(file_size, file_ptr, img_size, img_data);

  // TODO unfilter and remap pixels
  /* uint32_t byte_per_line = img_size / header.height; */
  /* for (uint32_t i = 0; i < img_size; i += byte_per_line) { */
  /*   uint8_t b = ((uint8_t *) img_data)[i]; */
  /*   printf("%d ", b); */
  /* } */

  struct image img = {
    .width      = header.width,
    .height     = header.height,
    .depth      = header.depth,
    .color_type = header.color_type,
    // .palette = ?
    .data       = img_data,
  };
  return img;
}



void free_image(const struct image *image) {
  free(image->data);
  LOG_DEBUG("Free image");
}
