/**
 * @file test-mfile.c
 * @brief Test for mfile function
 * @details
 */


#include <CUnit/Basic.h>

#include "mfile.h"



int init_test_mfile(void) {
  return 0;
}

int clean_test_mfile(void) {
  return 0;
}



void check_png_mfile(void) {
  const struct mfile file = map_file("test-suite/PngSuite.png");
  CU_ASSERT_TRUE(mfile_is_png(&file));
  unmap_file(&file);
}

void check_none_png_mfile(void) {
  const struct mfile file = map_file("test-suite/PngSuite.README");
  CU_ASSERT_FALSE(mfile_is_png(&file));
  unmap_file(&file);
}
