#include "test-image.h"

#include "chunk.h"
#include "image.h"
#include "color.h"



int init_test_image(void) {
  return 0;
}

int clean_test_image(void) {
  return 0;
}



void test_get_image(void) {

  const struct mfile file = map_file("suite/basn0g02.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 2);
  CU_ASSERT_EQUAL(img.sample, 1);
  CU_ASSERT_EQUAL(img.palette, NULL);
  
  free_image(&img);
  unmap_file(&file);
}


void test_image_basn0g08(void) {

  const struct mfile file = map_file("suite/basn0g08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 8);
  CU_ASSERT_EQUAL(img.sample, 1);
  CU_ASSERT_EQUAL(img.palette, NULL);

  uint8_t v = 0;
  int8_t inc = 1;
  
  // Go through each pixel which go from 0 to 255 back and forth
  for (uint8_t i = 0; i < 32; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      struct color c;
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red,   v);
      CU_ASSERT_EQUAL(c.green, v);
      CU_ASSERT_EQUAL(c.blue,  v);
      CU_ASSERT_EQUAL(c.alpha, 255);

      v += inc;
      
      if (v == 0) // v reach the lower bound, incresing next times
        inc = 1;
      else if (v == 255) // reach the upper bound, decreasing next
        inc = -1;
    }
  }

  free_image(&img);
  unmap_file(&file);
}


void test_image_basn2c16(void) {

  const struct mfile file = map_file("suite/basn2c16.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 16);
  CU_ASSERT_EQUAL(img.sample, 3);
  CU_ASSERT_EQUAL(img.palette, NULL);

  const uint16_t ref[] =
    {65535, 63421, 61307, 59193, 57079, 54965, 52851, 50737,
     48623, 46509, 44395, 42281, 40167, 38053, 35939, 33825,
     31710, 29596, 27482, 25368, 23254, 21140, 19026, 16912,
     14798, 12684, 10570,  8456,  6342,  4228,  2114,     0};
  
  for (uint8_t i = 0; i < 32; i++) {
    for (uint8_t j = 0; j < 32; j++) {
      
      struct color c;
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red, ref[j]); 
      CU_ASSERT_EQUAL(c.green, ref[i]);
      // compare blue to ref[] backward for the last i value
      CU_ASSERT_EQUAL(c.blue, (j < 32 - i ? 0 : ref[(31 - i) + (31 - j)]));
      CU_ASSERT_EQUAL(c.alpha, ref[0]);
    }
  }
  
  free_image(&img);
  unmap_file(&file);
}


void test_image_basn4a08(void) {

  const struct mfile file = map_file("suite/basn4a08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 8);
  CU_ASSERT_EQUAL(img.sample, 2);
  CU_ASSERT_EQUAL(img.palette, NULL);

  const uint16_t ref[] =
    {  0,   8,  16,  24,  32,  41,  49,  57,
      65,  74,  82,  90,  98, 106, 115, 123,
     131, 139, 148, 156, 164, 172, 180, 189,
     197, 205, 213, 222, 230, 238, 246, 255};   

  for (uint8_t i = 0; i < 32; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      struct color c;
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red, ref[31 - i]);
      CU_ASSERT_EQUAL(c.green, ref[31 - i]);
      CU_ASSERT_EQUAL(c.blue, ref[31 - i]);
      CU_ASSERT_EQUAL(c.alpha, ref[j]);
    }
  }
  
  free_image(&img);
  unmap_file(&file);
}


void test_image_pp0n6a08(void) {
  
  const struct mfile file = map_file("suite/pp0n6a08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);
  CU_ASSERT_EQUAL(img.depth, 8);
  CU_ASSERT_EQUAL(img.sample, 4);
  CU_ASSERT_EQUAL(img.palette, NULL);

  const uint16_t ref[] =
    {  0,   8,  16,  24,  32,  41,  49,  57,
      65,  74,  82,  90,  98, 106, 115, 123,
     131, 139, 148, 156, 164, 172, 180, 189,
     197, 205, 213, 222, 230, 238, 246, 255};   

  struct color c;
  int16_t value;
  
  value = 255;
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 32; j++) {
      get_color(&img, i, j, &c);
      CU_ASSERT_EQUAL(c.red, 255);
      CU_ASSERT_EQUAL(c.green, 255);
      CU_ASSERT_EQUAL(c.blue, value--);
      CU_ASSERT_EQUAL(c.alpha, ref[j]);
    }
  }
  value = 255;
  for (uint8_t i = 8; i < 16; i++) {
    for (uint8_t j = 0; j < 32; j++) {
      get_color(&img, i, j, &c);
      CU_ASSERT_EQUAL(c.red, 255);
      CU_ASSERT_EQUAL(c.green, value--);
      CU_ASSERT_EQUAL(c.blue, 255);
      CU_ASSERT_EQUAL(c.alpha, ref[j]);
    }
  }
  value = 255;
  for (uint8_t i = 16; i < 24; i++) {
    for (uint8_t j = 0; j < 32; j++) {
      get_color(&img, i, j, &c);
      CU_ASSERT_EQUAL(c.red, value--);
      CU_ASSERT_EQUAL(c.green, 255);
      CU_ASSERT_EQUAL(c.blue, 255);
      CU_ASSERT_EQUAL(c.alpha, ref[j]);
    }
  }
  value = 255;
  for (uint8_t i = 24; i < 32; i++) {
    for (uint8_t j = 0; j < 32; j++) {
      get_color(&img, i, j, &c);
      CU_ASSERT_EQUAL(c.red, value);
      CU_ASSERT_EQUAL(c.green, value);
      CU_ASSERT_EQUAL(c.blue, value);
      CU_ASSERT_EQUAL(c.alpha, ref[j]);
      value--;
    }
  }
  
  free_image(&img);
  unmap_file(&file);
}
