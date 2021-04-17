#include "test-filter.h"

#include "mfile.h"
#include "image.h"
#include "color.h"


/**
 * @brief Image without filter to compare to the other unfilter images
 * @details More precisely, compare the 8 first and 8 last line
 */
static struct image reference;


int init_test_filter(void) {
  const struct mfile file = map_file("suite/f00n0g08.png");
  reference = get_image(&file);
  unmap_file(&file);
  return 0;
}

int clean_test_filter(void) {
  free_image(&reference);
  return 0;
}


void test_filter_sub(void) {

  const struct mfile file = map_file("suite/f01n0g08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);

  struct color ref;
  struct color c;

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      get_color(&reference, i, j, &ref);
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red, ref.red);
      CU_ASSERT_EQUAL(c.green, ref.green);
      CU_ASSERT_EQUAL(c.blue, ref.blue);
      CU_ASSERT_EQUAL(c.alpha, ref.alpha);
    }
  }

  free_image(&img);
  unmap_file(&file);
}

void test_filter_up(void) {

  const struct mfile file = map_file("suite/f02n0g08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);

  struct color ref;
  struct color c;

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      get_color(&reference, i, j, &ref);
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red, ref.red);
      CU_ASSERT_EQUAL(c.green, ref.green);
      CU_ASSERT_EQUAL(c.blue, ref.blue);
      CU_ASSERT_EQUAL(c.alpha, ref.alpha);
    }
  }

  free_image(&img);
  unmap_file(&file);
}

void test_filter_average(void) {

  const struct mfile file = map_file("suite/f03n0g08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);

  struct color ref;
  struct color c;

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      get_color(&reference, i, j, &ref);
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red, ref.red);
      CU_ASSERT_EQUAL(c.green, ref.green);
      CU_ASSERT_EQUAL(c.blue, ref.blue);
      CU_ASSERT_EQUAL(c.alpha, ref.alpha);
    }
  }

  free_image(&img);
  unmap_file(&file);
}

void test_filter_paeth(void) {

  const struct mfile file = map_file("suite/f04n0g08.png");
  const struct image img  = get_image(&file);

  CU_ASSERT_EQUAL(img.width, 32);
  CU_ASSERT_EQUAL(img.height, 32);

  struct color ref;
  struct color c;

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 32; j++) {

      get_color(&reference, i, j, &ref);
      get_color(&img, i, j, &c);

      CU_ASSERT_EQUAL(c.red, ref.red);
      CU_ASSERT_EQUAL(c.green, ref.green);
      CU_ASSERT_EQUAL(c.blue, ref.blue);
      CU_ASSERT_EQUAL(c.alpha, ref.alpha);
    }
  }

  free_image(&img);
  unmap_file(&file);
}
