/**
 * @file viewer.h
 * @brief Display a struct image
 */

#ifndef __VIEWER_H__
#define __VIEWER_H__


#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>

#include "image.h"
#include "log.h"


/**
 * @brief Default background color
 */
static const uint8_t default_bg_color[3] = {255, 255, 255};


/**
 * @brief Display the image
 * @details Blocking until the window is closed
 * @param[in] image
 */
void view_image(const struct image *image);

/**
 * @brief Save image in a BMP file
 * @param[in] image
 * @param[in] filename name of the file to write
 */
void save_image_as_bmp(const struct image *image, const char *filename);


#endif // __VIEWER_H__
