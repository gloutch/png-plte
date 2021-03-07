
#include "chunk.h"


#define UINT32_FROM_PTR(ptr) (*((uint32_t *) (ptr)))
#define UINT16_FROM_PTR(ptr) (*((uint16_t *) (ptr)))
#define  UINT8_FROM_PTR(ptr)  (*((uint8_t *) (ptr)))



/** @brief Number of public chunk */
#define NB_PUBLIC_CHUNK (18)

/**
 * @brief Array of chunk type value
 * @details In the same order than enum chunk_type
 */
static const char * const public_type_value[NB_PUBLIC_CHUNK] = {
  "IHDR", "PLTE", "IDAT", "IEND", "tRNS", "gAMA", "cHRM", "sRGB", "iCCP",
  "tEXt", "zTXt", "iTXt", "bKGD", "pHYs", "sBIT", "sPLT", "hIST", "tIME"
};

/**
 * @brief Convert type value from png to enumerate value
 * @param[in] type Deserialized chunk type value
 * @return The corresponding enum value or UNKN
 */
static enum chunk_type chunk_type_value_to_enum(uint32_t type) {

  for (int i = 0; i < NB_PUBLIC_CHUNK; i++) {
    if (type == UINT32_FROM_PTR(public_type_value[i])) {
      LOG_TRACE("Converion type '%.4s' to enum %d", (char *) &type, i + 1);
      return i + 1;
    }
  }
  LOG_WARN("Unknown type: '%.4s'", (char *) &type);
  return UKWN;
}

uint32_t enum_to_type_value(enum chunk_type type) {
  assert(type != UKWN);
  return UINT32_FROM_PTR(public_type_value[type - 1]);
}




const struct chunk get_chunk(size_t size, const void *data) {
  if (size < 12) {
    LOG_FATAL("Remaind file too short to get a chunk: size %zu", size);
    exit(1);
  }
 
  const uint8_t *ptr = data;
  
  // length of the chunk data (first 4 bytes)
  uint32_t data_length = ntohl(UINT32_FROM_PTR(ptr));
  
  if (size < (8 + data_length + 4)) {
    LOG_FATAL("Remaind file too short (%zu) for the expected data length: %d", size, data_length);
    exit(1);
  }
  
  uint32_t expected_crc = ntohl(UINT32_FROM_PTR(ptr + 8 + data_length));
  uint32_t computed_crc = crc((unsigned char *) ptr + 4, 4 + data_length);
  if (expected_crc != computed_crc) {
    LOG_WARN("Chunk CRC 0x%x != computed 0x%x", expected_crc, computed_crc);
  }

  uint32_t chunk_type = UINT32_FROM_PTR(ptr + 4);
  
  const struct chunk res = {
    .length = data_length,
    .type   = chunk_type_value_to_enum(chunk_type),
    .data   = ptr + 8,
    .crc    = expected_crc,
  };
  LOG_INFO("%.4s, data %-6d crc 0x%x", (char *) &(chunk_type), res.length, res.crc);
  return res;
}




const struct IHDR IHDR_chunk(const struct chunk *chunk) {
  assert(chunk->type == IHDR);
  assert(chunk->length == 13);
  
  const uint8_t *ptr = chunk->data;
  const struct IHDR res = {
    .width       = ntohl(UINT32_FROM_PTR(ptr)),
    .height      = ntohl(UINT32_FROM_PTR(ptr + 4)),
    .depth       = UINT8_FROM_PTR(ptr + 8),
    .color_type  = UINT8_FROM_PTR(ptr + 9),
    .compression = UINT8_FROM_PTR(ptr + 10),
    .filter      = UINT8_FROM_PTR(ptr + 11),
    .interlace   = UINT8_FROM_PTR(ptr + 12),
  };

  // TODO: check values
  assert(res.compression == 0);
  assert(res.filter == 0);

  LOG_DEBUG("[%d,%d]  depth %d  color-type %d  compression %d  filter %d  interlace %d",
           res.width, res.height, res.depth, res.color_type, res.compression, res.filter, res.interlace);
  return res;
}



const struct PLTE PLTE_chunk(const struct chunk *chunk, const struct IHDR *header) {
  assert(chunk->type == PLTE);

  if ((chunk->length % 3) != 0) {
    LOG_FATAL("Wrong palette size: %d (not divisible by 3)", chunk->length);
    exit(1);
  }
  LOG_DEBUG("Pixel sample depth: %d (among 1,2,4,8)", header->depth);

  // compute number of color index
  uint16_t max_color = ((uint16_t) 1) << header->depth;
  uint32_t nb_color  = chunk->length / 3;

  if (nb_color > max_color) {
    LOG_WARN("Too much color (%d) compared to index range [0-%d] (depth %d)", nb_color, max_color - 1, header->depth); 
  }
  LOG_DEBUG("Palette index [0-%d]", nb_color - 1); 
  
  const struct PLTE res = {
    .nb_color = nb_color,
    .color    = chunk->data,
  };
  return res;
}



uint32_t GAMA_chunk(const struct chunk *chunk) {
  assert(chunk->type == GAMA);
  assert(chunk->length == 4);
  return ntohl(UINT32_FROM_PTR(chunk->data));
}



const struct BKGD BKGD_chunk(const struct chunk *chunk, const struct IHDR *header) {
  assert(chunk->type == BKGD);
  
  struct BKGD bg;
  bg.color_type = header->color_type;
  const uint8_t *ptr = chunk->data;
  
  switch (header->color_type) {
  case PLTE_INDEX: {
    
    assert(chunk->length == 1);
    bg.color.index = UINT8_FROM_PTR(ptr);
    return bg;
  }
  case GRAYSCALE:
  case GRAYSCALE_ALPHA: {
    
    assert(chunk->length == 2);
    bg.color.gray = ntohs(UINT16_FROM_PTR(ptr));
    return bg;
  }
  case RGB_TRIPLE:
  case RGB_TRIPLE_ALPHA: {
    
    assert(chunk->length == 6);
    bg.color.rgb.red   = ntohs(UINT16_FROM_PTR(ptr));
    bg.color.rgb.green = ntohs(UINT16_FROM_PTR(ptr + 2));
    bg.color.rgb.blue  = ntohs(UINT16_FROM_PTR(ptr + 4));
    return bg;
  }
  }
  assert(0);
}



const struct PHYS PHYS_chunk(const struct chunk *chunk) {
  assert(chunk->type == PHYS);
  assert(chunk->length == 9);

  const uint8_t *ptr = chunk->data;
  const struct PHYS res = {
    .x_axis = ntohl(UINT32_FROM_PTR(ptr)),
    .y_axis = ntohl(UINT32_FROM_PTR(ptr + 4)),
    .unit   = UINT8_FROM_PTR(ptr + 8),
  };
  return res;
}



const struct TIME TIME_chunk(const struct chunk *chunk) {
  assert(chunk->type == TIME);
  assert(chunk->length == 7);

  const uint8_t *ptr = chunk->data;
  const struct TIME res = {
    .year   = ntohs(UINT16_FROM_PTR(ptr)),
    .month  = UINT8_FROM_PTR(ptr + 2),
    .day    = UINT8_FROM_PTR(ptr + 3),
    .hour   = UINT8_FROM_PTR(ptr + 4),
    .minute = UINT8_FROM_PTR(ptr + 5),
    .second = UINT8_FROM_PTR(ptr + 6),
  };
  return res;
}
