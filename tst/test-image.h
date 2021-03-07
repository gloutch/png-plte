#ifndef __TEST_IMAGE_H__
#define __TEST_IMAGE_H__

#include <CUnit/Basic.h>
#include <stdint.h>


int init_test_image(void);

int clean_test_image(void);



void test_bit_per_pixel(void);

void test_byte_per_line(void);

void test_get_image(void);


#endif // __TEST_IMAGE_H__
