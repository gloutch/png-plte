/**
 * @file filter.h
 * @brief Filter/Unfilter a raw image
 * @details
 */

#ifndef __FILTER_H__
#define __FILTER_H__


#include <stdint.h>
#include <stdlib.h>

#include "log.h"
#include "image.h"


/**
 * @brief Unfilter the whole image
 * @param[in,out] image Raw unpack image to unfilter
 */
void unfilter(struct image *image);



#endif // __FILTER_H__
