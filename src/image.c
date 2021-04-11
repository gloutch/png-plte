
#include "image.h"
#include "chunk.h"
#include "filter.h"


/** @brief Size of the square pattern */
#define ADAM7_SIZE (8)
/** @brief Number of pass int the Adam7 interlacing method */
#define ADAM7_PASS (7)

/**
 * @brief Interlacing Adam7 layout
 * @details This data structure must help working with interlaced image
 */
struct adam7 {
  /** @brief Pattern for [interlacing](http://www.libpng.org/pub/png/spec/1.2/PNG-DataRep.html#DR.Interlaced-data-order) */
  const uint8_t *pattern;
  /** @brief Pointer to the passes (including filter byte) */
  uint8_t *pass[ADAM7_PASS];
  /** @brief Width of the passes (in pixel) */
  uint32_t width[ADAM7_PASS];
  /** @brief Height of the passes (in pixel) */
  uint32_t height[ADAM7_PASS];
};

/**
 * @brief Adam7 layout pattern
 */
static const uint8_t adam7_pattern[ADAM7_SIZE * ADAM7_SIZE] =
  {0, 5, 3, 5, 1, 5, 3, 5,
   6, 6, 6, 6, 6, 6, 6, 6,
   4, 5, 4, 5, 4, 5, 4, 5,
   6, 6, 6, 6, 6, 6, 6, 6,
   2, 5, 3, 5, 2, 5, 3, 5,
   6, 6, 6, 6, 6, 6, 6, 6,
   4, 5, 4, 5, 4, 5, 4, 5,
   6, 6, 6, 6, 6, 6, 6, 6};

/**
 * @brief Set info about the interlacing layout
 * @param[in] width
 * @param[in] height
 * @param[out] info
 */
static void pass_layout(uint32_t width, uint32_t height, struct adam7 *info) {
  info->pattern = adam7_pattern;
  // pass 0: divide by 8 (round up)
  info->width[0]  = (width + 7) / 8;
  info->height[0] = (height + 7) / 8;
  // pass 1:
  info->width[1]  = (width + 7 - 4) / 8; // starting at index 4
  info->height[1] = (height + 7) / 8;
  // pass 2:
  info->width[2]  = (width + 3) / 4;
  info->height[2] = (height + 7 - 4) / 8;
  // pass 3:
  info->width[3]  = ((width + 7 - 2) / 8) + ((width + 7 - 6) / 8);
  info->height[3] = (height + 3) / 4;
  // pass 4:
  info->width[4]  = (width + 1) / 2;
  info->height[4] = ((height + 7 - 2) / 8) + ((height + 7 - 6) / 8);
  // pass 5:
  info->width[5]  = (width + 1 - 1) / 2;
  info->height[5] = (height + 1) / 2;
  // pass 6:
  info->width[6]  = width;
  info->height[6] = (height + 1 - 1) / 2;

  LOG_DEBUG("[%d,%d]  ->  passes  0:[%d,%d]  1[%d,%d]  2[%d,%d]  3[%d,%d]  4[%d,%d]  5[%d,%d]  6[%d,%d]", width, height,
            info->width[0], info->height[0], info->width[1], info->height[1], info->width[2], info->height[2],
            info->width[3], info->height[3], info->width[4], info->height[4], info->width[5], info->height[5],
            info->width[6], info->height[6]);
}



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
 * @brief Compute the length of a scanline (in byte)
 * @param[in] depth Count of bit per sample
 * @param[in] sample Count of sample per pixel
 * @param[in] width Count of pixel
 * @return Number of byte for a pixel scanline in the image
 */
static uint32_t byte_per_line(uint8_t depth, uint8_t sample, uint32_t width) {
  uint32_t bit_per_line = depth * sample * width;
  return (bit_per_line + 7) / 8; // round up to one if % 8 != 0
}

/**
 * @brief Compute the needed size for the decompression of IDAt chunk
 * @details Add one [filter-type byte](http://www.libpng.org/pub/png/spec/1.2/PNG-DataRep.html#DR.Filtering)
 * to each scanline to get the total size
 * @param[in] header Header chunk of the imahe
 * @return size
 */
static uint32_t unpack_size(const struct IHDR *header) {
  assert((header->interlace == 0) || (header->interlace == 1));

  if (header->interlace == 0) {
    // no interlacing
    return header-> height * (1 + byte_per_line(header->depth, count_sample(header->color_type), header->width));
  }
  else {
    // interlace Adam7
    uint32_t size = 0;
    struct adam7 passes;
    pass_layout(header->width, header->height, &passes);

    for (uint8_t p = 0; p < ADAM7_PASS; p++) { // iterate on passes
      if (passes.width[p] * passes.height[p]) {
        size += passes.height[p] * (1 + byte_per_line(header->depth, count_sample(header->color_type), passes.width[p]));
      }
    }
    return size;
  }
}

/**
 * @brief Consume all IDAT chunk to inflate all image data (using zlib only in this function)
 * @details IDAT chunk must be [consecutive](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.Summary-of-standard-chunks)
 * @param[in] file_size
 * @param[in] file_ptr Allocated file content (pointer to the first IDAT chunk)
 * @param[in] img_size Size allocated starting at img_ptr
 * @param[out] img_ptr Pointer to the area to fill with unpack data
 */
static void unpack_data(uint32_t file_size, const uint8_t *file_ptr, uint32_t img_size, void *img_ptr) {
  // the first IDAT chunk of the file
  struct chunk current = get_chunk(file_size, file_ptr);
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

  // skip the 8 byte signature
  uint32_t file_size = file->size - 8;
  uint8_t *file_ptr  = ((uint8_t *) file->data) + 8;
  // get the header chunk
  struct chunk current = get_chunk(file_size, file_ptr);
  const struct IHDR header = IHDR_chunk(&current);

  // limitation
  if (header.color_type == PLTE_INDEX) {
    LOG_FATAL("Color type (PLTE) not handle YET");
    exit(1);
  }

  // compute the size of the unpack image
  uint32_t img_size = unpack_size(&header);
  void    *img_data = malloc(img_size);
  if (img_data == NULL) {
    LOG_FATAL("Can't allocate %d byte to get the image", img_size);
    exit(1);
  }
  LOG_DEBUG("Malloc(%d) from %p", img_size, img_data);

  // find the first IDAT chunk
  while (current.type != IDAT) {
    file_size -= (current.length + 12); // chunk size
    file_ptr  += (current.length + 12);
    current = get_chunk(file_size, file_ptr);
  }
  unpack_data(file_size, file_ptr, img_size, img_data); // inflate image

  if (header.interlace == 1) {
    LOG_FATAL("Interlacing methode (ADAM7) no handle YET");
    exit(1);
  }

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
  LOG_WARN("Free image %p", image->data);
  free(image->data);
}

