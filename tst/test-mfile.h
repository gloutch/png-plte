/**
 * @file test-mfile.c
 * @brief Test for mfile function
 * @details
 */

#ifndef __TEST_MFILE_H__
#define __TEST_MFILE_H__

#include <CUnit/Basic.h>



int init_test_mfile(void);

int clean_test_mfile(void);


void check_png_mfile(void);

void check_none_png_mfile(void);



#endif // __TEST_MFILE_H__
