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
 * @brief Display the image
 * @details Blocking until the window is closed
 * @param[in] image
 */
void view_image(const struct image *image);



#endif // __VIEWER_H__
