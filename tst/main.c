/**
 * @file main.c
 * @brief The main test
 * @details
 */


#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "log.h"
#include "test-mfile.h"



CU_pSuite add_suite(const char* strName, CU_InitializeFunc pInit, CU_CleanupFunc pClean) {
  CU_pSuite suite = CU_add_suite(strName, pInit, pClean);
  if (NULL == suite) {
    CU_cleanup_registry();
    CU_ErrorCode code = CU_get_error();
    exit(code);
  }
  return suite;
}

void add_test(CU_pSuite pSuite, const char* strName, CU_TestFunc pTestFunc) {
  if (NULL == CU_add_test(pSuite, strName, pTestFunc)) {
    CU_cleanup_registry();
    CU_ErrorCode code = CU_get_error();
    exit(code);
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
   

   /* Run all tests using the CUnit Basic interface */
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
