#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "chunk.h"
#include "filter.h"
#include "image.h"
#include "log.h"




/** @brief Size of the square pattern */
#define ADAM7_SIZE (8)

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




/**
 * @brief Consume all IDAT chunk to inflate all image data (using zlib only in this function)
 * @details IDAT chunk must be [consecutive](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.Summary-of-standard-chunks)
 * @param[in] fsize Remaining size of the file starting at fptr
 * @param[in] fptr Allocated file content (pointer to the first IDAT chunk)
 * @param[in] isize Size allocated from iptr
 * @param[out] iptr Pointer to the area to fill with unpack data
 */
static void unpack_IDAT(uint32_t fsize, const uint8_t *fptr, uint32_t isize, void *iptr) {
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
  stream.next_out  = iptr;
  stream.avail_out = isize;
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
      else if (err != Z_OK) {
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
 * @details The no interlace version is quite easy to understand, however with adam7...
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
  const uint32_t unpack_size = hdr->height * (1 + lsize); // adding the filter type-byte per scanline

  // malloc
  void *data = malloc(unpack_size);
  if (data == NULL) {
    LOG_FATAL("Can't malloc(%d) to unpack image", unpack_size);
    exit(1);
  }
  LOG_ALLOC("Malloc(%d) at %p", unpack_size, data);

  // unpack
  unpack_IDAT(fsize, fptr, unpack_size, data);

  // unfilter
  uint8_t bpp = (hdr->depth * sample + 7) / 8;
  unfilter(data, 1 + lsize, hdr->height, bpp);

  // remap pixels
  for (uint8_t i = 0; i < hdr->height; i++) {
    uint8_t *src = data + (i * (1 + lsize)) + 1;
    uint8_t *dst = data + (i * lsize);
    memcpy(dst, src, lsize);
  }

  // data
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
 * @brief Unpack IDAT chunk, unfilter each passes from an interlace (ADAM7) image
 * @param[in] header Header chunk of the file
 * @param[in] fsize Size after fptr
 * @param[in] fptr Pointer to file content (first IDAT chunk)
 * @param[out] pass Pointer to 7 images
 */
static void passes_from_IDAT_adam7(const struct IHDR *hdr, uint32_t fsize, const uint8_t *fptr, struct image pass[ADAM7_NB_PASS]) {
  assert(hdr->interlace == 1); // adam7

  // needed constants, compute sizes
  pass[0].width  = (hdr->width + 7) / 8; // divide by 8 (round up to one)
  pass[0].height = (hdr->height + 7) / 8;
  pass[1].width  = (hdr->width + 7 - 4) / 8; // same and starting at index 4
  pass[1].height = (hdr->height + 7) / 8;
  pass[2].width  = (hdr->width + 3) / 4;
  pass[2].height = (hdr->height + 7 - 4) / 8;
  pass[3].width  = ((hdr->width + 7 - 2) / 8) + ((hdr->width + 7 - 6) / 8);
  pass[3].height = (hdr->height + 3) / 4;
  pass[4].width  = (hdr->width + 1) / 2;
  pass[4].height = ((hdr->height + 7 - 2) / 8) + ((hdr->height + 7 - 6) / 8);
  pass[5].width  = (hdr->width + 1 - 1) / 2;
  pass[5].height = (hdr->height + 1) / 2;
  pass[6].width  = hdr->width;
  pass[6].height = (hdr->height + 1 - 1) / 2;

  LOG_INFO("[%d,%d]  ->  1[%d,%d]  2[%d,%d]  3[%d,%d]  4[%d,%d]  5[%d,%d]  6[%d,%d]  7[%d,%d]",
            hdr->width, hdr->height, pass[0].width, pass[0].height,
            pass[1].width, pass[1].height, pass[2].width, pass[2].height, pass[3].width, pass[3].height,
            pass[4].width, pass[4].height, pass[5].width, pass[5].height, pass[6].width, pass[6].height);

  const uint8_t sample = count_sample(hdr->color_type);
  uint32_t unpack_size = 0;
  uint32_t lsize[ADAM7_NB_PASS]; // lenght (in byte) of the line

  for (uint8_t p = 0; p < ADAM7_NB_PASS; p++) {
    // check for empty passes
    if (pass[p].width * pass[p].height > 0) {
      // sum sizes
      lsize[p] = byte_per_line(hdr->depth, sample, pass[p].width);
      unpack_size += pass[p].height * (1 + lsize[p]); // add the filter type-byte
    }
    else {
      pass[p].width = 0;
      pass[p].height = 0;
      // (lsize[p] == 0) => empty pass
      lsize[p] = 0;
    }
    // use this first loop to init passes
    pass[p].depth   = hdr->depth;
    pass[p].sample  = sample;

  }

  // malloc
  void *unpack = malloc(unpack_size);
  if (unpack == NULL) {
    LOG_FATAL("Can't malloc(%d) to unpack image", unpack_size);
    exit(1);
  }
  LOG_ALLOC("Malloc(%d) packed interlace img %p", unpack_size, unpack);
  unpack_IDAT(fsize, fptr, unpack_size, unpack); // unpack

  // unfilter, remap, data
  uint8_t bpp = (hdr->depth * sample + 7) / 8;
  uint8_t *ptr = unpack; // current ptr to pass

  for (uint8_t p = 0; p < ADAM7_NB_PASS; p++) {
    
    if (lsize[p] == 0) {
      // empty pass
      pass[p].palette = NULL;
      pass[p].data    = NULL;
      LOG_INFO("Pass %d empty", p + 1);
    }
    else {
      // unflter
      unfilter(ptr, 1 + lsize[p], pass[p].height, bpp);
      // remap
      for (uint32_t i = 0; i < pass[p].height; i++) {
        uint8_t *src = ptr + (i * (1 + lsize[p])) + 1;
        uint8_t *dst = ptr + (i * lsize[p]);
        memcpy(dst, src, lsize[p]);
      }
      // data
      pass[p].palette = NULL;
      pass[p].data    = ptr;
      // set ptr to next pass
      ptr += pass[p].height * (1 + lsize[p]);
      LOG_INFO("Pass %d done", p + 1);
    }
  }
}





uint32_t line_size(const struct image *image) {
  return byte_per_line(image->depth, image->sample, image->width);
}


const struct image get_image(const struct mfile *file) {
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
  if (header.interlace == 1) {
    LOG_FATAL("Interlace ADAM7 not handle YET");
    exit(1);
  }

  // find the first IDAT chunk
  while (current.type != IDAT) {
    fsize -= (current.length + 12); // chunk size
    fptr  += (current.length + 12);
    current = get_chunk(fsize, fptr);
  }
  return image_from_IDAT(&header, fsize, fptr);
}


void free_image(const struct image *image) {
  LOG_ALLOC("Free image %p", image->data);
  free(image->data);
}



void get_adam7_passes(const struct mfile *file, struct image pass[ADAM7_NB_PASS]) {
  assert(mfile_is_png(file) == 1);

  // skip the 8 byte signature
  uint32_t fsize = file->size - 8;
  uint8_t *fptr  = ((uint8_t *) file->data) + 8;

  // get the header chunk
  struct chunk current = get_chunk(fsize, fptr);
  const struct IHDR header = IHDR_chunk(&current);

  if (header.interlace != 1) {
    LOG_FATAL("Ask to get passes from a non interlaced (ADAM7) image %s", file->pathname);
    exit(1);
  }

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
  passes_from_IDAT_adam7(&header, fsize, fptr, pass);
}


void free_passes(struct image pass[ADAM7_NB_PASS]) {
  for (uint8_t p = 0; p < ADAM7_NB_PASS; p++) {
    // free the first non-NULL pass[p].data
    if (pass[p].data != NULL) {
      LOG_ALLOC("Free %p", pass[p].data);
      free(pass[p].data);
      return;
    }
  }
}
