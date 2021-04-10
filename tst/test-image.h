#ifndef __TEST_IMAGE_H__
#define __TEST_IMAGE_H__

#include <CUnit/Basic.h>
#include <stdint.h>


int init_test_image(void);

int clean_test_image(void);



void test_get_image(void);

/* Test an entier image for each color type */

void test_image_basn0g08(void);

void test_image_basn2c16(void);

// TODO type 3

void test_image_basn4a08(void);

void test_image_pp0n6a08(void);


#endif // __TEST_IMAGE_H__
