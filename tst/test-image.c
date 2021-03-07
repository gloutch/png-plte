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
