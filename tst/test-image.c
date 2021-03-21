#include "test-image.h"

#include "chunk.h"
#include "image.h"



int init_test_image(void) {
  return 0;
}

int clean_test_image(void) {
  return 0;
}



void test_get_image(void) {

  const struct mfile file = map_file("suite/basn0g08.png");
  const struct image img  = image_from_png(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 8);
  CU_ASSERT_EQUAL(img.sample, 1);
  CU_ASSERT_EQUAL(img.palette, NULL);
  
  free_image(&img);
  unmap_file(&file);
}

void test_image_basn0g08(void) {

  const struct mfile file = map_file("suite/basn0g08.png");
  const struct image img  = image_from_png(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 8);
  CU_ASSERT_EQUAL(img.sample, 1);
  CU_ASSERT_EQUAL(img.palette, NULL);

  uint8_t v = 0;
  int8_t inc = 1;
  
  // Go through each pixel which go from 0 to 255 back and forth
  for (uint8_t i = 0; i < 32; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      struct color c;
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red,   v);
      CU_ASSERT_EQUAL(c.green, v);
      CU_ASSERT_EQUAL(c.blue,  v);
      CU_ASSERT_EQUAL(c.alpha, 255);

      v += inc;
      
      if (v == 0) // v reach the lower bound, incresing next times
        inc = 1;
      else if (v == 255) // reach the upper bound, decreasing next
        inc = -1;
    }
  }

  free_image(&img);
  unmap_file(&file);
}
