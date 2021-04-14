/**
 * @file filter.h
 * @brief Filter/Unfilter a raw image
 * @details
 */

#ifndef __FILTER_H__
#define __FILTER_H__

#include <stdint.h>


/**
 * @brief Unfilter the consecutive scanline of same length
 * @param[in,out] data Pointer to the first scanline
 * (includinf the [filter type-byte](http://www.libpng.org/pub/png/spec/1.2/PNG-DataRep.html#DR.Filtering))
 * @param[in] length Length of a scanline (including the filter type-byte)
 * @param[in] height Number of scanline
 * @param[in] bpp Byte per pixel (round up to one)
 */
void unfilter(uint8_t *data, uint32_t length, uint32_t height, uint8_t bpp);


#endif // __FILTER_H__
