/**
 * @file image.h
 * @brief Get the image data from a PNG mfile
 * @details From a whole PNG mfile, get the IHDR and needed chunk to extract the image data from IDAT chunk.
 * The idea is to be independent from the PNG chunk structures and play only with pixels and colors now.
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdint.h>

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
  /** @brief Number of sample of pixel */
  uint8_t sample;
  /** @brief PLTE pointer or NULL (no control on the palette size) */
  uint8_t *palette;
  /** @brief Image data (pixels or index) */
  void *data;
};

/**
 * @brief Length of a scanline
 * @param[in] image
 * @return Length in byte
 */
uint32_t line_size(const struct image *image);



/**
 * @brief From PNG file to the actual image
 * @param[in] file A PNG file which may be free right after
 * @return The image
 */
const struct image get_image(const struct mfile *file);

/**
 * @brief Free the image
 * @param[in] image The image to free
 */
void free_image(const struct image *image);



/** @brief Number of passes in the Adam7 interlace */
#define ADAM7_NB_PASS (7U)

/**
 * @brief Get the 7 passes from an interlace image
 * @datails Empty passes have .with = 0, .height = 0, .palette = NULL .data = NULL
 * @param[in] file A PNG file with Adam7 interlace
 * @param[out] pass Each passes (Do not free each image one by one, use free_passes)
 */
void get_adam7_passes(const struct mfile *file, struct image pass[ADAM7_NB_PASS]);

/**
 * @brief Free the image
 * @param[in] image The image to free
 */
void free_passes(struct image pass[ADAM7_NB_PASS]);



#endif // __IMAGE_H__ 
