#include "chunk.h"
#include "mfile.h"

#include "test-chunk.h"



int init_test_chunk(void) {
  return 0;
}

int clean_test_chunk(void) {
  return 0;
}

static const struct IHDR get_header(const struct mfile *file) {
  const struct chunk header = get_chunk(file->size - 8, ((uint8_t *)file->data) + 8);
  return IHDR_chunk(&header);
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
  uint32_t offset = 8 + 25 + 16;
  
  const struct mfile file1 = map_file("suite/cm0n0g04.png");
  const struct chunk chunk1 = get_chunk(file1.size - offset, ((uint8_t * )file1.data) + offset);
  CU_ASSERT_EQUAL(chunk1.type, TIME);
  const struct TIME time1 = TIME_chunk(&chunk1);

  CU_ASSERT_EQUAL(time1.year, 2000);
  CU_ASSERT_EQUAL(time1.month, 1);
  CU_ASSERT_EQUAL(time1.day, 1);
  unmap_file(&file1);
  

  const struct mfile file2 = map_file("suite/cm7n0g04.png");
  const struct chunk chunk2 = get_chunk(file2.size - offset, ((uint8_t * )file2.data) + offset);
  CU_ASSERT_EQUAL(chunk2.type, TIME);
  const struct TIME time2 = TIME_chunk(&chunk2);

  CU_ASSERT_EQUAL(time2.year, 1970);
  CU_ASSERT_EQUAL(time2.month, 1);
  CU_ASSERT_EQUAL(time2.day, 1);
  unmap_file(&file2);
}


void test_gamma(void) {
  uint32_t offset = 8 + 25;
  
  const struct mfile file1 = map_file("suite/g07n2c08.png");
  const struct chunk chunk1 = get_chunk(file1.size - offset, ((uint8_t * )file1.data) + offset);
  CU_ASSERT_EQUAL(chunk1.type, GAMA);
  uint32_t gamma1 = GAMA_chunk(&chunk1);

  CU_ASSERT_EQUAL(gamma1, 70000);
  unmap_file(&file1);

  
  const struct mfile file2 = map_file("suite/g03n2c08.png");
  const struct chunk chunk2 = get_chunk(file2.size - offset, ((uint8_t * )file2.data) + offset);
  CU_ASSERT_EQUAL(chunk2.type, GAMA);
  uint32_t gamma2 = GAMA_chunk(&chunk2);

  CU_ASSERT_EQUAL(gamma2, 35000);
  unmap_file(&file2);


  const struct mfile file3 = map_file("suite/g05n0g16.png");
  const struct chunk chunk3 = get_chunk(file3.size - offset, ((uint8_t * )file3.data) + offset);
  CU_ASSERT_EQUAL(chunk3.type, GAMA);
  uint32_t gamma3 = GAMA_chunk(&chunk3);

  CU_ASSERT_EQUAL(gamma3, 55000);
  unmap_file(&file3);
}


void test_physic(void) {
  uint32_t offset = 8 + 25 + 16 + 15;
  
  const struct mfile file1 = map_file("suite/cdfn2c08.png");
  const struct chunk chunk1 = get_chunk(file1.size - offset, ((uint8_t *)file1.data) + offset);
  CU_ASSERT_EQUAL(chunk1.type, PHYS);
  const struct PHYS phys1 = PHYS_chunk(&chunk1);
  
  CU_ASSERT_EQUAL(phys1.x_axis, 1);
  CU_ASSERT_EQUAL(phys1.y_axis, 4);
  CU_ASSERT_EQUAL(phys1.unit,   0);
  unmap_file(&file1);


  const struct mfile file2 = map_file("suite/cdun2c08.png");
  const struct chunk chunk2 = get_chunk(file1.size - offset, ((uint8_t *)file1.data) + offset);
  CU_ASSERT_EQUAL(chunk2.type, PHYS);
  const struct PHYS phys2 = PHYS_chunk(&chunk2);
  
  CU_ASSERT_EQUAL(phys2.x_axis, 1000);
  CU_ASSERT_EQUAL(phys2.y_axis, 1000);
  CU_ASSERT_EQUAL(phys2.unit,   1);
  unmap_file(&file2);
}


void test_bkgd(void) {
  uint32_t offset = 8 + 25 + 16;

  const struct mfile file1 = map_file("suite/bgwn6a08.png");
  const struct IHDR header1 = get_header(&file1);
  const struct chunk chunk1 = get_chunk(file1.size - offset, ((uint8_t *)file1.data) + offset);
  CU_ASSERT_EQUAL(chunk1.type, BKGD);
  const struct BKGD bkgd1 = BKGD_chunk(&chunk1, &header1);

  CU_ASSERT_EQUAL(bkgd1.color_type, RGB_TRIPLE_ALPHA);
  CU_ASSERT_EQUAL(bkgd1.color.rgb.red,   255);
  CU_ASSERT_EQUAL(bkgd1.color.rgb.green, 255);
  CU_ASSERT_EQUAL(bkgd1.color.rgb.blue,  255);
  unmap_file(&file1);


  const struct mfile file2 = map_file("suite/bgyn6a16.png");
  const struct IHDR header2 = get_header(&file2);
  const struct chunk chunk2 = get_chunk(file2.size - offset, ((uint8_t *)file2.data) + offset);
  CU_ASSERT_EQUAL(chunk2.type, BKGD);
  const struct BKGD bkgd2 = BKGD_chunk(&chunk2, &header2);

  CU_ASSERT_EQUAL(bkgd2.color_type, RGB_TRIPLE_ALPHA);
  CU_ASSERT_EQUAL(bkgd2.color.rgb.red,   65535);
  CU_ASSERT_EQUAL(bkgd2.color.rgb.green, 65535);
  CU_ASSERT_EQUAL(bkgd2.color.rgb.blue,  0);
  unmap_file(&file2);


  const struct mfile file3 = map_file("suite/bgbn4a08.png");
  const struct IHDR header3 = get_header(&file3);
  const struct chunk chunk3 = get_chunk(file3.size - offset, ((uint8_t *)file3.data) + offset);
  CU_ASSERT_EQUAL(chunk3.type, BKGD);
  const struct BKGD bkgd3 = BKGD_chunk(&chunk3, &header3);

  CU_ASSERT_EQUAL(bkgd3.color_type, GRAYSCALE_ALPHA);
  CU_ASSERT_EQUAL(bkgd3.color.gray, 0);
  unmap_file(&file3);
}
