#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>


int init_suite1(void)
{
  return 0;
}

int clean_suite1(void)
{
  return 0;
}

void testFPRINTF(void)
{
  printf("bof");
}

void testFREAD(void)
{
  CU_ASSERT_TRUE(0);
}



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


int main()
{
   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();


   CU_pSuite pSuite1 = add_suite("Suite_1", init_suite1, clean_suite1);
   add_test(pSuite1, "test of fprintf()", testFPRINTF);
   add_test(pSuite1, "test of fread()", testFREAD);
   

   /* Run all tests using the CUnit Basic interface */
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
