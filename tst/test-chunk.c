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


void test_header(void) {

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


void test_time(void) {

  const struct mfile file1 = map_file("suite/cm0n0g04.png");
  const struct chunk chunk1 = get_chunk(file1.size - 8, ((uint8_t * )file1.data) + 8 + 25 + 16);
  CU_ASSERT_EQUAL(chunk1.type, TIME);
  const struct TIME time1 = TIME_chunk(&chunk1);

  CU_ASSERT_EQUAL(time1.year, 2000);
  CU_ASSERT_EQUAL(time1.month, 1);
  CU_ASSERT_EQUAL(time1.day, 1);
  unmap_file(&file1);
  

  const struct mfile file2 = map_file("suite/cm7n0g04.png");
  const struct chunk chunk2 = get_chunk(file2.size - 8, ((uint8_t * )file2.data) + 8 + 25 + 16);
  CU_ASSERT_EQUAL(chunk2.type, TIME);
  const struct TIME time2 = TIME_chunk(&chunk2);

  CU_ASSERT_EQUAL(time2.year, 1970);
  CU_ASSERT_EQUAL(time2.month, 1);
  CU_ASSERT_EQUAL(time2.day, 1);
  unmap_file(&file2);

}


void test_gamma(void) {

  const struct mfile file1 = map_file("suite/g07n2c08.png");
  const struct chunk chunk1 = get_chunk(file1.size - 8, ((uint8_t * )file1.data) + 8 + 25);
  CU_ASSERT_EQUAL(chunk1.type, GAMA);
  uint32_t gamma1 = GAMA_chunk(&chunk1);

  CU_ASSERT_EQUAL(gamma1, 70000);
  unmap_file(&file1);

  
  const struct mfile file2 = map_file("suite/g03n2c08.png");
  const struct chunk chunk2 = get_chunk(file2.size - 8, ((uint8_t * )file2.data) + 8 + 25);
  CU_ASSERT_EQUAL(chunk2.type, GAMA);
  uint32_t gamma2 = GAMA_chunk(&chunk2);

  CU_ASSERT_EQUAL(gamma2, 35000);
  unmap_file(&file2);


  const struct mfile file3 = map_file("suite/g05n0g16.png");
  const struct chunk chunk3 = get_chunk(file3.size - 8, ((uint8_t * )file3.data) + 8 + 25);
  CU_ASSERT_EQUAL(chunk3.type, GAMA);
  uint32_t gamma3 = GAMA_chunk(&chunk3);

  CU_ASSERT_EQUAL(gamma3, 55000);
  unmap_file(&file3);
}
