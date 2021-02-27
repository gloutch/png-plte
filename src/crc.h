/**
 * @file crc.h
 * @brief CRC function
 * @details The source comes from [pnglib](http://www.libpng.org/pub/png/spec/1.2/PNG-CRCAppendix.html)
 */

#ifndef __CRC_H__
#define __CRC_H__

/**
 * @brief Compute de CRC
 * @param[in] buf pointer to the start of the byte area
 * @param[in] len length of the buf area
 * @return the CRC of the bytes buf[0..len-1]
 */
unsigned long crc(const unsigned char *buf, int len);


#endif // __CRC_H__
