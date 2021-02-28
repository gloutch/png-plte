#include "chunk.h"
#include "mfile.h"

#include "test-chunk.h"



int init_test_chunk(void) {
  return 0;
}

int clean_test_chunk(void) {
  return 0;
}



void get_chunk_from_ptr(void) {

  const struct mfile file = map_file("suite/basi0g01.png");
  const struct chunk header = get_chunk(file.size - 8, ((uint8_t * )file.data) + 8);
  
  CU_ASSERT_EQUAL(header.length, 13);
  CU_ASSERT_EQUAL(header.type, IHDR);
  CU_ASSERT_EQUAL(header.data, ((uint8_t * )file.data) + 16);
  CU_ASSERT_EQUAL(header.crc, 0x2c0677cf);
  
  unmap_file(&file);
}


void get_header_from_ptr(void) {

  const struct mfile file = map_file("suite/basn0g01.png"); // no interlace
  const struct chunk chunk = get_chunk(file.size - 8, ((uint8_t * )file.data) + 8);
  CU_ASSERT_EQUAL(chunk.type, IHDR);
  const struct IHDR header = IHDR_chunk(&chunk);

  CU_ASSERT_EQUAL(header.width, 32);
  CU_ASSERT_EQUAL(header.height, 32);
  CU_ASSERT_EQUAL(header.depth, 1);
  CU_ASSERT_EQUAL(header.color_type, GRAYSCALE);
  CU_ASSERT_EQUAL(header.compression, 0);
  CU_ASSERT_EQUAL(header.filter, 0);
  CU_ASSERT_EQUAL(header.interlace, 0);
  
  unmap_file(&file);
}
