#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "chunk.h"
#include "filter.h"
#include "image.h"
#include "log.h"



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
 * @return Number of byte for a scanline in the image
 */
static uint32_t byte_per_line(uint8_t depth, uint8_t sample, uint32_t width) {
  uint32_t bit_per_line = depth * sample * width;
  return (bit_per_line + 7) / 8; // round up to one if % 8 != 0
}



/** @brief Size of the square pattern */
#define ADAM7_SIZE (8)
/** @brief Number of pass int the Adam7 interlacing method */
#define ADAM7_PASS (7)

/**
 * @brief Adam7 layout [pattern](http://www.libpng.org/pub/png/spec/1.2/PNG-DataRep.html#DR.Interlaced-data-order)
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
 * @brief Interlacing Adam7 layout
 * @details This data structure must help working with interlaced image
 */
struct adam7 {
  /** @brief Width of the passes (in pixel) */
  uint32_t width[ADAM7_PASS];
  /** @brief Height of the passes (in pixel) */
  uint32_t height[ADAM7_PASS];
};

/**
 * @brief Set info about the interlacing layout
 * @param[in] header
 * @return adam7 structure
 */
static const struct adam7 adam7_pass_layout(const struct IHDR *header) {
  LOG_INFO("Adam7 layout from [%d,%d] img", header->width, header->height);
  struct adam7 p;

  // pass 0
  p.width[0]  = (header->width + 7) / 8; // divide by 8 (round up to one)
  p.height[0] = (header->height + 7) / 8;
  // pass 1
  p.width[1]  = (header->width + 7 - 4) / 8; // same and starting at index 4
  p.height[1] = (header->height + 7) / 8;
  // pass 2
  p.width[2]  = (header->width + 3) / 4;
  p.height[2] = (header->height + 7 - 4) / 8;
  // pass 3
  p.width[3]  = ((header->width + 7 - 2) / 8) + ((header->width + 7 - 6) / 8);
  p.height[3] = (header->height + 3) / 4;
  // pass 4
  p.width[4]  = (header->width + 1) / 2;
  p.height[4] = ((header->height + 7 - 2) / 8) + ((header->height + 7 - 6) / 8);
  // pass 5
  p.width[5]  = (header->width + 1 - 1) / 2;
  p.height[5] = (header->height + 1) / 2;
  // pass 6
  p.width[6]  = header->width;
  p.height[6] = (header->height + 1 - 1) / 2;

  LOG_DEBUG("[%d,%d]  ->  passes  0:[%d,%d]  1[%d,%d]  2[%d,%d]  3[%d,%d]  4[%d,%d]  5[%d,%d]  6[%d,%d]", header->width, header->height,
            p.width[0], p.height[0], p.width[1], p.height[1], p.width[2], p.height[2], p.width[3], p.height[3],
            p.width[4], p.height[4], p.width[5], p.height[5], p.width[6], p.height[6]);
  return p;
}



/**
 * @brief Consume all IDAT chunk to inflate all image data (using zlib only in this function)
 * @details IDAT chunk must be [consecutive](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.Summary-of-standard-chunks)
 * @param[in] fsize Remaining size of the file starting at fptr
 * @param[in] fptr Allocated file content (pointer to the first IDAT chunk)
 * @param[in] img_size Size allocated starting at img_ptr
 * @param[out] img_ptr Pointer to the area to fill with unpack data
 */
static void unpack_data(uint32_t fsize, const uint8_t *fptr, uint32_t img_size, void *img_ptr) {
  // the first IDAT chunk of the file
  struct chunk current = get_chunk(fsize, fptr);
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
    fsize -= (current.length + 12);
    fptr  += (current.length + 12);
    current = get_chunk(fsize, fptr);

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


/**
 * @brief Unpack IDAT chunk, unfilter image (NO interlace image)
 * @param[in] header Header chunk of the file
 * @param[in] fsize Size after fptr
 * @param[in] fptr Pointer to file content (first IDAT chunk)
 * @return The final image
 */
static struct image image_from_IDAT(const struct IHDR *hdr, uint32_t fsize, const uint8_t *fptr) {
  assert(hdr->interlace == 0); // no interlace

  // needed constants, compute unpack size
  const uint8_t sample = count_sample(hdr->color_type); // number of sample in a pixel
  const uint32_t lsize = byte_per_line(hdr->depth, sample, hdr->width); // length of a line
  const uint32_t unpack_size = hdr->height * (1 + lsize);

  // malloc
  void *data = malloc(unpack_size);
  if (data == NULL) {
    LOG_FATAL("Can't malloc(%d) to unpack image", unpack_size);
    exit(1);
  }
  LOG_ALLOC("Malloc(%d) at %p", unpack_size, data);

  // unpack
  unpack_data(fsize, fptr, unpack_size, data);

  // unfilter
  uint8_t bpp = (hdr->depth * sample + 7) / 8;
  unfilter(data, 1 + lsize, hdr->height, bpp);

  // remap pixels
  for (uint8_t i = 0; i < hdr->height; i++) {
    uint8_t *src = data + (i * (1 + lsize)) + 1;
    uint8_t *dst = data + (i * lsize);
    memcpy(dst, src, lsize);
  }

  // image
  struct image r = {
    .width   = hdr->width,
    .height  = hdr->height,
    .depth   = hdr->depth,
    .sample  = sample,
    .palette = NULL,
    .data    = data,
  };
  return r;
}


/**
 * @brief Unpack IDAT chunk, unfilter image (interlace ADAM7)
 * @param[in] header Header chunk of the file
 * @param[in] fsize Size after fptr
 * @param[in] fptr Pointer to file content (first IDAT chunk)
 * @return The final image
 */
static struct image image_from_IDAT_adam7(const struct IHDR *hdr, uint32_t fsize, const uint8_t *fptr) {
  assert(hdr->interlace == 1);

  // needed constants, compute unpack size
  const uint8_t sample = count_sample(hdr->color_type);
  const struct adam7 interlace = adam7_pass_layout(hdr);

  uint32_t unpack_size = 0;
  uint32_t lsize[ADAM7_PASS];

  for (uint8_t i = 0; i < ADAM7_PASS; i++) {
    lsize[i] = byte_per_line(hdr->depth, sample, interlace.width[i]);
    unpack_size += interlace.height[i] * (1 + lsize[i]); // add the filter type-byte
  }

  // malloc
  void *unpack = malloc(unpack_size);
  if (unpack == NULL) {
    LOG_FATAL("Can't malloc(%d) to unpack image", unpack_size);
    exit(1);
  }
  LOG_ALLOC("Malloc(%d) packed interlace img %p", unpack_size, unpack);

  // unpack
  unpack_data(fsize, fptr, unpack_size, unpack);

  // unfilter
  uint8_t *pass[ADAM7_PASS];
  uint8_t bpp = (hdr->depth * sample + 7) / 8;

  pass[0] = unpack;
  unfilter(unpack, 1 + lsize[0], interlace.height[0], bpp);

  for (uint8_t i = 1; i < ADAM7_PASS; i++) {
    pass[i] = pass[i - 1] + interlace.height[i - 1] * (1 + lsize[i - 1]);
    unfilter(pass[i], 1 + lsize[i], interlace.height[i], bpp);
  }

  // compute new size
  uint32_t bwidth = lsize[6];
  void *data = malloc(bwidth * hdr->height);
  if (data == NULL) {
    LOG_FATAL("Can't malloc(%d) to remap adam7 interlace pixel", bwidth * hdr->height);
    exit(1);
  }
  LOG_ALLOC("Malloc(%d) image %p", bwidth * hdr->height, data);

  // remap pixels
  // TODO six first passes
  for (uint32_t i = 1; i < hdr->height; i += 2) { // last pass is easy to remap
    uint8_t *src = pass[6] + ((i / 2) * (1 + lsize[6])) + 1;
    uint8_t *dst = data + (i * bwidth);
    memcpy(dst, src, bwidth);
  }
  LOG_ALLOC("Free %p", unpack);
  free(unpack);

  // image
  struct image r = {
    .width   = hdr->width,
    .height  = hdr->height,
    .depth   = hdr->depth,
    .sample  = sample,
    .palette = NULL,
    .data    = data,
  };
  return r;
}



const struct image image_from_png(const struct mfile *file) {
  assert(mfile_is_png(file) == 1);

  // skip the 8 byte signature
  uint32_t fsize = file->size - 8;
  uint8_t *fptr  = ((uint8_t *) file->data) + 8;

  // get the header chunk
  struct chunk current = get_chunk(fsize, fptr);
  const struct IHDR header = IHDR_chunk(&current);


  // limitation
  if (header.color_type == PLTE_INDEX) {
    LOG_FATAL("Color type (PLTE) not handle YET");
    exit(1);
  }

  // find the first IDAT chunk
  while (current.type != IDAT) {
    fsize -= (current.length + 12); // chunk size
    fptr  += (current.length + 12);
    current = get_chunk(fsize, fptr);
  }


  if (header.interlace == 0) {
    return image_from_IDAT(&header, fsize, fptr);
  }
  else {
    return image_from_IDAT_adam7(&header, fsize, fptr);
  }
}



uint32_t line_size(const struct image *image) {
  return byte_per_line(image->depth, image->sample, image->width);
}



void free_image(const struct image *image) {
  LOG_ALLOC("Free image %p", image->data);
  free(image->data);
}
