#ifndef __TEST_CHUNK_H__
#define __TEST_CHUNK_H__

#include <CUnit/Basic.h>
#include <stdint.h>


int init_test_chunk(void);

int clean_test_chunk(void);


void chunk_type_enum_convertion(void);

void get_chunk_from_ptr(void);

void get_header_from_ptr(void);


#endif // __TEST_CHUNK_H__
