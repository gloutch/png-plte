#include "color.h"


/**
 * @brief Compute the pointer to the pixel (i, j)
 * @details A sample hold on at least one byte, which make easier to compute the pointer
 * @param[in] image
 * @param[in] i
 * @param[in] j
 * @return the pointer
 */
static void *pixel_pointer(const struct image *image, uint32_t i, uint32_t j) {
  assert((image->depth == 8) || (image->depth == 16));

  uint8_t psize = (image->depth / 8) * image->sample;
  uint32_t lsize = psize * image->width;

  return image->data + (i * (lsize + 1)) + 1 + (psize * j);
}

/**
 * @brief Compute the pointer to the byte holding the pixel (i, j)
 * @details A sample is less than a byte, so the pixel is in the bit, left_shift bits from the left 
 * @param[in] image
 * @param[in] i
 * @param[in] j
 * @param[out] left_shift
 * @return the pointer
 */
static uint8_t *sample_pointer(const struct image *image, uint32_t i, uint32_t j, uint8_t *left_shift) {
  assert(image->depth != 16);

  uint32_t lsize = line_size(image);
  uint32_t bit_shift = image->depth * image->sample * j;

  *left_shift = bit_shift % 8;
  return image->data + (i * (lsize + 1)) + 1 + (bit_shift / 8);
}

/**
 * @brief Extract the sample from one byte
 * @param[in] byte The byte where the sample is
 * @param[in] shift Shift in bits from the left to get the sample (0 <= shift < 8)
 * @param[in] depth Size of the sample in bits (shift + depth <= 8)
 * @param[out] max Max value of the sample
 * @return The sample value on a byte
 */
static uint8_t get_sample(uint8_t byte, uint8_t left_shift, uint8_t depth, uint8_t *max) {
  // set the last depth bits to 1
  uint8_t mask = (((uint16_t) 1) << depth) - 1;
  // shift from the right to get the sample
  uint8_t right_shift = 8 - left_shift - depth;

  *max = mask;
  return (byte >> right_shift) & mask;
}


void get_color(const struct image *image, uint32_t i, uint32_t j, struct color *color) {
  assert((0 <= i) && (i < image->height));
  assert((0 <= j) && (j < image->width));

  if (image->palette != NULL) { // PLTE: depth 1 2 4 8
    uint8_t max;
    uint8_t left_shift;
    uint8_t *ptr = sample_pointer(image, i, j, &left_shift);
    uint8_t index = get_sample(*ptr, left_shift, image->depth, &max);

    uint8_t *pixel = image->palette + (index * 3);

    color->red   = pixel[0];
    color->green = pixel[1];
    color->blue  = pixel[2];
    color->alpha = max;
    color->max   = max;
    return;
  }

  switch (image->sample) {

  case 1: // Grayscale
    if (image->depth < 8) { // depth 1 2 4
      uint8_t max;
      uint8_t left_shift;
      uint8_t *ptr = sample_pointer(image, i, j, &left_shift);
      uint8_t gray = get_sample(*ptr, left_shift, image->depth, &max);

      color->red   = gray;
      color->green = gray;
      color->blue  = gray;
      color->alpha = max;
      color->max   = max;
    } else if (image->depth == 8) { // depth 8
      uint8_t * ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[0];
      color->blue  = ptr[0];
      color->alpha = 0xff;
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      uint16_t gray = ntohs(ptr[0]);
      color->red   = gray;
      color->green = gray;
      color->blue  = gray;
      color->alpha = 0xffff;
      color->max   = 0xffff;
    }
    break;
  case 2: // Grayscale + Alpha
    if (image->depth == 8) { // depth 8
      uint8_t *ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[0];
      color->blue  = ptr[0];
      color->alpha = ptr[1];
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      uint16_t gray = ntohs(ptr[0]);
      color->red   = gray;
      color->green = gray;
      color->blue  = gray;
      color->alpha = ntohs(ptr[1]);
      color->max   = 0xffff;
    }
    break;
  case 3: // RGB
    if (image->depth == 8) { // depth 8
      uint8_t *ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[1];
      color->blue  = ptr[2];
      color->alpha = 0xff;
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      color->red   = ntohs(ptr[0]);
      color->green = ntohs(ptr[1]);
      color->blue  = ntohs(ptr[2]);
      color->alpha = 0xffff;
      color->max   = 0xffff;
    }
    break;
  case 4: // RGB + Alpha
    if (image->depth == 8) { // depth 8
      uint8_t *ptr = pixel_pointer(image, i, j);
      color->red   = ptr[0];
      color->green = ptr[1];
      color->blue  = ptr[2];
      color->alpha = ptr[3];
      color->max   = 0xff;
    } else { // depth 16
      uint16_t *ptr = pixel_pointer(image, i, j);
      color->red   = ntohs(ptr[0]);
      color->green = ntohs(ptr[1]);
      color->blue  = ntohs(ptr[2]);
      color->alpha = ntohs(ptr[3]);
      color->max   = 0xffff;
    }
    break;
  default:
    assert(0);
  }
}
