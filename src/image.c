
#include "image.h"
#include "chunk.h"


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

  if (bit_per_line % 8 == 0) {
    // no bit wasted
    return bit_per_line / 8;
  }
  // add one last byte
  LOG_INFO("The last %d bits are wasted on each scanline", 8 - (bit_per_line % 8));
  return (bit_per_line / 8) + 1;
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

  // inflate end
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

  // TODO:unfilter

  struct image img = {
    .width   = header.width,
    .height  = header.height,
    .depth   = header.depth,
    .sample  = count_sample(header.color_type),
    .palette = NULL,
    .data    = img_data,
  };
  return img;
}



uint32_t line_size(const struct image *image) {
  return byte_per_line(image->depth, image->sample, image->width);
}



void free_image(const struct image *image) {
  free(image->data);
  LOG_DEBUG("Free image");
}
