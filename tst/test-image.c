#include "test-image.h"

#include "chunk.h"
#include "image.h"


int init_test_image(void) {
  return 0;
}

int clean_test_image(void) {
  return 0;
}



void test_bit_per_pixel(void) {
  // http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html#C.IHDR
  
  CU_ASSERT_EQUAL(bit_per_pixel(1,  GRAYSCALE), 1);
  CU_ASSERT_EQUAL(bit_per_pixel(2,  GRAYSCALE), 2);
  CU_ASSERT_EQUAL(bit_per_pixel(4,  GRAYSCALE), 4);
  CU_ASSERT_EQUAL(bit_per_pixel(8,  GRAYSCALE), 8);
  CU_ASSERT_EQUAL(bit_per_pixel(16, GRAYSCALE), 16);

  CU_ASSERT_EQUAL(bit_per_pixel(8,  RGB_TRIPLE), 24);
  CU_ASSERT_EQUAL(bit_per_pixel(16, RGB_TRIPLE), 48);
  
  CU_ASSERT_EQUAL(bit_per_pixel(1, PLTE_INDEX), 1);
  CU_ASSERT_EQUAL(bit_per_pixel(2, PLTE_INDEX), 2);
  CU_ASSERT_EQUAL(bit_per_pixel(4, PLTE_INDEX), 4);
  CU_ASSERT_EQUAL(bit_per_pixel(8, PLTE_INDEX), 8);
  
  CU_ASSERT_EQUAL(bit_per_pixel(8,  GRAYSCALE_ALPHA), 16);
  CU_ASSERT_EQUAL(bit_per_pixel(16, GRAYSCALE_ALPHA), 32);

  CU_ASSERT_EQUAL(bit_per_pixel(8,  RGB_TRIPLE_ALPHA), 32);
  CU_ASSERT_EQUAL(bit_per_pixel(16, RGB_TRIPLE_ALPHA), 64);
}


void test_byte_per_line(void) {

  CU_ASSERT_EQUAL(byte_per_line(1, GRAYSCALE, 8), 1);
  CU_ASSERT_EQUAL(byte_per_line(1, GRAYSCALE, 9), 2);
  
  CU_ASSERT_EQUAL(byte_per_line(8,  RGB_TRIPLE, 2), 6);
  CU_ASSERT_EQUAL(byte_per_line(16, RGB_TRIPLE, 2), 12);

  CU_ASSERT_EQUAL(byte_per_line(2, PLTE_INDEX, 4), 1);
  CU_ASSERT_EQUAL(byte_per_line(2, PLTE_INDEX, 5), 2);
  CU_ASSERT_EQUAL(byte_per_line(4, PLTE_INDEX, 2), 1);
  CU_ASSERT_EQUAL(byte_per_line(4, PLTE_INDEX, 3), 2);
  CU_ASSERT_EQUAL(byte_per_line(8, PLTE_INDEX, 1), 1);
  CU_ASSERT_EQUAL(byte_per_line(8, PLTE_INDEX, 2), 2);

  CU_ASSERT_EQUAL(byte_per_line(8,  GRAYSCALE_ALPHA, 2), 4);
  CU_ASSERT_EQUAL(byte_per_line(8,  GRAYSCALE_ALPHA, 3), 6);
  
  CU_ASSERT_EQUAL(byte_per_line(16, RGB_TRIPLE_ALPHA, 2), 16);
  CU_ASSERT_EQUAL(byte_per_line(16, RGB_TRIPLE_ALPHA, 3), 24);
}


void test_get_image(void) {

  const struct mfile file = map_file("suite/basn0g08.png");
  const struct image img  = image_from_file(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 8);
  CU_ASSERT_EQUAL(img.color_type, GRAYSCALE);
  
  free_image(&img);
  unmap_file(&file);
}
