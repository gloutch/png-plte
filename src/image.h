/**
 * @file image.h
 * @brief Get the image data from a PNG mfile
 * @details From a whole PNG mfile, get the IHDR and needed chunk to extract image data from IDAT.
 * The idea is to work only with the image struct and not longer with the PNG file
 */


#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <zlib.h>

#include "log.h"
#include "chunk.h"
#include "mfile.h"


/**
 * @brief Image struct
 */
struct image {
  /** @brief Width of the image (number of colomn) */
  uint32_t width;
  /** @brief Height of the image (number of line) */
  uint32_t height;
  /** @brief Number of bits per sample or palette index */
  uint8_t depth;
  /** @brief Color type from PNG file */
  enum color_type color_type;
  /** @brief PNG palette (color_tyle == PLTE_INDEX) */
  const struct PLTE palette;
  /** @brief Image data */
  void *data;
};

/**
 * @brief Compute bits per pixel (one pixel may be a palette index)
 * @param[in] depth Number of bits per sample or palette index
 * @param[in] type Color type of the image
 * @return bit per pixel
 */
uint8_t bit_per_pixel(uint8_t depth, enum color_type type);

/**
 * @brief Compute the length of a scanline
 * @param[in] depth
 * @param[in] type
 * @param[in] width
 * @return Number of byte for a pixel (or index) line in the image
 */
uint32_t byte_per_line(uint8_t depth, enum color_type type, uint32_t width);



/**
 * @brief From PNG file to the actual image
 * @param[in] file A PNG file which may be free right after
 * @return The image
 */
const struct image image_from_file(const struct mfile *file);

/**
 * @brief Free the image
 * @param[in] image The image to free
 */
void free_image(const struct image *image);


#endif // __IMAGE_H__ 
