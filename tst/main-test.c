/**
 * @file main-test.c
 * @brief The main test
 * @details
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "log.h"
#include "test-mfile.h"
#include "test-crc.h"
#include "test-chunk.h"
#include "test-image.h"
#include "test-filter.h"


CU_pSuite add_suite(const char* strName, CU_InitializeFunc pInit, CU_CleanupFunc pClean) {
  CU_pSuite suite = CU_add_suite(strName, pInit, pClean);
  if (NULL == suite) {
    CU_cleanup_registry();
    exit(CU_get_error());
  }
  return suite;
}

void add_test(CU_pSuite pSuite, const char* strName, CU_TestFunc pTestFunc) {
  if (NULL == CU_add_test(pSuite, strName, pTestFunc)) {
    CU_cleanup_registry();
    exit(CU_get_error());
  }
}


int main() {
  LOG_LOG_LEVEL();
   
  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();
   
   
  CU_pSuite pSuite1 = add_suite("mfile", init_test_mfile, clean_test_mfile);
  add_test(pSuite1, "check mfile_is_png success", check_png_mfile);
  add_test(pSuite1, "check mfile_is_png failed", check_none_png_mfile);
   
  CU_pSuite pSuite2 = add_suite("CRC", init_test_crc, clean_test_crc);
  add_test(pSuite2, "compute a CRC once", compute_crc);
   
  CU_pSuite pSuite3 = add_suite("Chunk", init_test_chunk, clean_test_chunk);
  add_test(pSuite3, "Chunk from file content", get_chunk_from_ptr);
  add_test(pSuite3, "Header chunk", test_header);
  add_test(pSuite3, "Time chunk", test_time);
  add_test(pSuite3, "Gamma chunk", test_gamma);
  add_test(pSuite3, "Physical size chunk", test_physic);
  add_test(pSuite3, "Background chunk", test_bkgd);
  add_test(pSuite3, "Palette chunk", test_plte);
   
  CU_pSuite pSuite4 = add_suite("Image", init_test_image, clean_test_image);
  add_test(pSuite4, "Image from file", test_get_image);
  add_test(pSuite4, "Image pixel per pixel basn0g08.png", test_image_basn0g08);
  add_test(pSuite4, "Image pixel per pixel basn2c16.png", test_image_basn2c16);
  add_test(pSuite4, "Image pixel per pixel basn4a08.png", test_image_basn4a08);
  add_test(pSuite4, "Image pixel per pixel pp0n6a08.png", test_image_pp0n6a08);

  CU_pSuite pSuite5 = add_suite("Filter", init_test_filter, clean_test_filter);
  add_test(pSuite5, "Sub (1)", test_filter_sub);
  add_test(pSuite5, "Up (2)", test_filter_up);
  add_test(pSuite5, "Average (3)", test_filter_average);
  add_test(pSuite5, "Paeth (4)", test_filter_paeth);
   
  /* Run all tests using the CUnit Basic interface */
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
