#ifndef __TEST_CHUNK_H__
#define __TEST_CHUNK_H__

#include <CUnit/Basic.h>
#include <stdint.h>


int init_test_chunk(void);

int clean_test_chunk(void);



void get_chunk_from_ptr(void);

void test_header(void);

void test_time(void);

void test_gamma(void);


#endif // __TEST_CHUNK_H__