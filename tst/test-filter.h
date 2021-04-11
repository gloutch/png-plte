#ifndef __TEST_FILTER_H__
#define __TEST_FILTER_H__

#include <CUnit/Basic.h>
#include <stdint.h>


int init_test_filter(void);

int clean_test_filter(void);


void test_filter_sub(void);

void test_filter_up(void);

void test_filter_average(void);

void test_filter_paeth(void);


#endif // __TEST_FILTER_H__
