#ifndef __TEST_IMAGE_H__
#define __TEST_IMAGE_H__

#include <CUnit/Basic.h>
#include <stdint.h>


int init_test_image(void);

int clean_test_image(void);



void test_get_image(void);

void test_image_basn0g08(void);

void test_image_basn2c16(void);


#endif // __TEST_IMAGE_H__
