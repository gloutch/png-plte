/**
 * @file test-crc.c
 * @brief Small test for crc
 * @details
 */


#include "test-crc.h"
#include "crc.h"



int init_test_crc(void) {
  return 0;
}

int clean_test_crc(void) {
  return 0;
}


// This is the smallest test ever, and not that relevant
void compute_crc(void) {
  unsigned char value = 0;
  CU_ASSERT_EQUAL(crc(&value, sizeof(value)), 3523407757);
}
