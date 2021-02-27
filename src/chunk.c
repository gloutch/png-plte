
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



enum chunk_type chunk_type_value_to_enum(uint32_t type) {
  LOG_DEBUG("Converion type '%.4s' to enum", (char *) &type);

  for (int i = 0; i < NB_PUBLIC_CHUNK; i++) {
    if (type == UINT32_FROM_PTR(public_type_value[i])) {
      return i + 1;
    }
  }
  LOG_WARN("Unknown type: '%.4s'", (char *) &type);
  return UKWN;
}


/**
 * @brief Convert enum chunk_type to chunt type value
 * @param[in] type (!= UKWN)
 * @return Type value
 */
static uint32_t enum_to_chunk_type_value(enum chunk_type type) {
  assert(type != UKWN);
  return UINT32_FROM_PTR(public_type_value[type - 1]);
}



const struct chunk get_chunk(size_t size, const void *data) {
  if (size < 12) {
    LOG_FATAL("Data too small to get a chunk: size %zu", size);
    exit(1);
  }
 
  const uint8_t *ptr = data;
  
  // length of the chunk data (first 4 bytes)
  uint32_t data_length = ntohl(UINT32_FROM_PTR(ptr));
  
  if (size < (8 + data_length + 4)) {
    LOG_FATAL("Data too small (%zu) for the expected data length: %d", size, data_length);
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
  LOG_INFO("Get chunk %.4s, data %d byte, crc 0x%x", (char *) &(chunk_type), res.length, res.crc);
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
  if (res.interlace != 0) {
    LOG_FATAL("Sorry, interlace methode not handled yet: %d", res.interlace);
    exit(1);
  }

  LOG_INFO("[%d,%d]  depth %d  color-type %d  compressoin %d  filter %d  interlace %d",
           res.width, res.height, res.depth, res.color_type, res.compression, res.filter, res.interlace);
  return res;
}



