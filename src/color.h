/**
 * @file color.h
 * @brief Functions to handle different color format in a image
 * @details
 */

#ifndef __COLOR_H__
#define __COLOR_H__

#include <assert.h>

#include "image.h"


/**
 * @brief General structure for color
 */
struct color {
  /** @brief Red channel */
  uint16_t red;
  /** @brief Green channel */
  uint16_t green;
  /** @brief Blue channel */
  uint16_t blue;
  /** @brief Alpha channel (0 = full transparent, max = full opaque) */
  uint16_t alpha;
  /** @brief Maximum value (0 is black, maw is white) */
  uint16_t max;
};

/**
 * @brief Get the color on the pixel (i, j) on the image
 * @param[in] image
 * @param[in] i Line of the pixel to get
 * @param[in] j Column of the pixel
 * @param[out] color The color to fill
 */
void get_color(const struct image *image, uint32_t i, uint32_t j, struct color *color);



#endif // __COLOR_H__
