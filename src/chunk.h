/**
 * @file chunk.h
 * @brief Functions to extract chunk
 * @details
 */


#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <stddef.h>
#include <stdint.h>


// generic chunk layout

/**
 * @brief Chunk type enumerate
 * @details I prefer enumerate value, at least for switch cases 
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
 * @brief Convert type value from png to enumerate value
 * @param[in] type Deserialized chunk type value
 * @return The corresponding enum value or UNKN
 */
enum chunk_type chunk_type_value_to_enum(uint32_t type);

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
 * @return IHDR chunk
 */
const struct IHDR IHDR_chunk(const struct chunk *chunk);



// chunk palette

/**
 * @brief Palette chunk
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.PLTE)
 */
struct PLTE {
  /** @brief Number of color on the palette (1 to 256) */
  uint16_t nb_color;
  /** @brief Pointer to the first color RGB on 1 byte each */
  const uint8_t *color;
};

/**
 * @brief Get the PLTE chunk
 * @param[in] chunk
 * @param[in] header Needed to check palette size
 * @return PLTE chunk
 */
const struct PLTE PLTE_chunk(const struct chunk *chunk, const struct IHDR *header);



// chunk gamma

/**
 * @brief Get Gamma value
 * @param[in] chunk
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.gAMA)
 * @return GAMA chunk
 */
uint32_t GAMA_chunk(const struct chunk *chunk);



// chunk background

/**
 * @brief Background color
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.bKGD)
 */
struct BKGD {
  /** @brief Color type from the header chunk (needed to guess the background format) */
  enum color_type color_type;
  /** @brief Union of format for colors */
  union {
    /** @brief Index of the color in PLTE */
    uint8_t index;
    /** @brief Single gray color */
    uint16_t gray;
    /** @brief RGB value */
    struct {
      /** @brief Red value */
      uint16_t red;
      /** @brief Green value */
      uint16_t green;
      /** @brief Blue value */
      uint16_t blue;
    } rgb;
  } color;
};

/**
 * @brief Get the background chunk
 * @param[in] chunk
 * @param[in] header The IHDR chunk (needed for color type and depth)
 * @return BKGD chunk
 */
const struct BKGD BKGD_chunk(const struct chunk *chunk, const struct IHDR *header);



// chunk phys

/**
 * @brief Physical pixel dimension
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.pHYs)
 */
struct PHYS {
  /** @brief Pixel per unit, X axis */
  uint32_t x_axis;
  /** @brief Pixel per unit, Y axis */
  uint32_t y_axis;
  /** @brief Unit specifier */
  uint8_t unit;
};

/**
 * @brief Get the physical chunk
 * @param[in] chunk
 * @return PHYS chunk
 */
const struct PHYS PHYS_chunk(const struct chunk *chunk);



// chunk time

/**
 * @brief Time chunk gives the time of the last modification
 * @details [Source](http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.tIME)
 */
struct TIME {
  /** @brief Year of the last modification */
  uint16_t year;
  /** @brief Month of the last modification */
  uint8_t month;
  /** @brief Day of the last modification */
  uint8_t day;
  /** @brief Hour of the last modification */
  uint8_t hour;
  /** @brief Minute of the last modification */
  uint8_t minute;
  /** @brief Second of the last modification */
  uint8_t second;
};

/**
 * @brief Get the time chunk
 * @param[in] chunk
 * @return TIME chunk
 */
const struct TIME TIME_chunk(const struct chunk *chunk);




#endif // __CHUNK_H__
