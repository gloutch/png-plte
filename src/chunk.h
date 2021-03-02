/**
 * @file chunk.h
 * @brief Functions to extract chunk
 * @details
 */


#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "log.h"
#include "crc.h"



// generic chunk layout

/**
 * @brief Chunk type enumerate
 * @details I prefer enumerate value at least for switch cases 
 */
enum chunk_type {
  /** @brief Type 0 is unknown chunk type */
  UKWN = 0,
  /** @brief [Image header](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.IHDR) */
  IHDR = 1,
  /** @brief [Palette](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.PLTE) */
  PLTE = 2,
  /** @brief [Image data](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.IDAT) */
  IDAT = 3,
  /** @brief [Image end](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.IEND) */
  IEND = 4,
  /** @brief [Transparency](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.tRNS) */
  TRNS = 5,
  /** @brief [Image gamma](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.gAMA) */
  GAMA = 6,
  /** @brief [Primay chromaticities](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.cHRM) */
  CHRM = 7,
  /** @brief [Standard RCB color space](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.sRGB) */
  SRGB = 8,
  /** @brief [Embedded ICC profile](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.iCCP) */
  ICCP = 9,
  /** @brief [Textual data](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.tEXt) */
  TEXT = 10,
  /** @brief [Compressed textual data](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.zTXt) */
  ZTXT = 11,
  /** @brief [International textual data](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.iTXt) */
  ITXT = 12,
  /** @brief [Background color](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.bKGD) */
  BKGD = 13,
  /** @brief [Physical pixel dimensions](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.pHYs) */
  PHYS = 14,
  /** @brief [Significant bits](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.sBIT) */
  SBIT = 15,
  /** @brief [Suggested palette](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.sPLT) */
  SPLT = 16,
  /** @brief [Palette histogram](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.hIST) */
  HIST = 17,
  /** @brief [Image last-modification time](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.tIME) */
  TIME = 18,
};

/**
 * @brief Convert enum chunk_type to chunk type value
 * @param[in] type (!= UKWN)
 * @return Type value
 */
uint32_t enum_to_type_value(enum chunk_type type);

/**
 * @brief Generic [chunk layout](http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html#Chunk-layout)
 */
struct chunk {
  /** @brief Length of the field .data (the size of a chunk is .length + 12) */
  const uint32_t length;
  /** @brief Type of the chunk */
  enum chunk_type type;
  /** @brief Pointer to the mapped data */
  const void *data;
  /** @brief CRC of the field type and data */
  const uint32_t crc;
};

/**
 * @brief Extract the chunk pointed by data
 * @param[in] size Max size of the data starting at data
 * @param[in] data Content from a PNG file
 * @result A generic chunk
 */
const struct chunk get_chunk(size_t size, const void *data);



// chunk header

/**
 * @brief PNG color type
 */
enum color_type {
  /** @brief Each pixel is a grayscale sample */
  GRAYSCALE = 0,
  /** @brief Each pixel is an RGB triple */
  RGB_TRIPLE = 2,
  /** @brief Index of a color in a palette */
  PLTE_INDEX = 3,
  /** @brief Grayscale + alpha */
  GRAYSCALE_ALPHA = 4,
  /** @brief RGB triple + alpha */
  RGB_TRIPLE_ALPHA = 6,
};

/**
 * @brief PNG header
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.IHDR)
 */
struct IHDR {
  /** @brief Width of the image */
  uint32_t width;
  /** @brief Height of the image */
  uint32_t height;
  /** @brief Depth of the image */
  uint8_t depth;
  /** @brief Color type */
  enum color_type color_type;
  /** @brief Compression method */
  uint8_t compression;
  /** @brief Filter method */
  uint8_t filter;
  /** @brief Interlace method */
  uint8_t interlace;
};

/**
 * @brief Get the header from a chunk
 * @param[in] chunk
 * @return Header chunk
 */
const struct IHDR IHDR_chunk(const struct chunk *chunk);



// chunk gamma

/**
 * @brief Get Gamma value
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.gAMA)
 */
uint32_t GAMA_chunk(const struct chunk *chunk);



// chunk time

/**
 * @brief Time chunk gives the time of the last modification
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.tIME)
 */
struct TIME {
  /** @brief Year */
  uint16_t year;
  /** @brief Month */
  uint8_t month;
  /** @brief Day */
  uint8_t day;
  /** @brief Hour */
  uint8_t hour;
  /** @brief Minute */
  uint8_t minute;
  /** @brief Second */
  uint8_t second;
};

/**
 * @brief Get the time chunk
 * @param[in] chunk
 * @return Time chunk
 */
const struct TIME TIME_chunk(const struct chunk *chunk);



#endif // __CHUNK_H__