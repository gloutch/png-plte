
#include "filter.h"


/**
 * @brief [sub](http://www.libpng.org/pub/png/spec/1.2/PNG-Filters.html#Filter-type-1-Sub) 
 */
static void sub_unfilter(uint32_t size, uint8_t *raw, uint8_t bpp) {

  for (uint32_t i = bpp; i < size; i++) {
    raw[i] += raw[i - bpp];
  }
}

/**
 * @brief [up](http://www.libpng.org/pub/png/spec/1.2/PNG-Filters.html#Filter-type-2-Up)
 */
static void up_unfilter(uint32_t size, uint8_t *raw, const uint8_t *prior, uint8_t bpp) {

  if (prior == NULL) {
    return;
  }
  for (uint32_t i = 0; i < size; i++) {
    raw[i] += prior[i];
  }
}

/**
 * @brief [average](http://www.libpng.org/pub/png/spec/1.2/PNG-Filters.html#Filter-type-3-Average)
 */
static void average_unfilter(uint32_t size, uint8_t *raw, const uint8_t *prior, uint8_t bpp) {
	
  if (prior == NULL) {

    for (uint32_t i = bpp; i < size; i++) {
      raw[i] += raw[i - bpp] / 2;
    }
  } else {

    for (uint32_t i = 0; i < bpp; i++) {
      raw[i] += prior[i] / 2;
    }
    for (uint32_t i = bpp; i < size; i++) {
      raw[i] += (raw[i - bpp] + prior[i]) / 2;
    }
  }
}

/**
 * @brief Absolute function of (p - a)
 */
#define ABS(p, a) (((p) >= (a)) ? ((p) - (a)) : ((a) - (p)))

/**
 * @brief The paeth predictor function
 */
static uint8_t paeth_predictor(const uint8_t left, const uint8_t above, const uint8_t upper_left) {

  int16_t a = left;
  int16_t b = above;
  int16_t c = upper_left;

  const int16_t p = a + b - c; // no overflow

  a = ABS(p, a);
  b = ABS(p, b);
  c = ABS(p, c);

  if ((a <= b) && (a <= c)) {
    return left;
  } else if (b <= c) {
    return above;
  } else {
    return upper_left;
  }
}

/**
 * @brief [paeth](http://www.libpng.org/pub/png/spec/1.2/PNG-Filters.html#Filter-type-4-Paeth)
 */
static void paeth_unfilter(uint32_t size, uint8_t *raw, const uint8_t *prior, uint8_t bpp) {

  if (prior == NULL) {
    for (uint32_t i = bpp; i < size; i++) {
      raw[i] += raw[i - bpp];
    }
    return;
  }

  for (uint32_t i = 0; i < bpp; i++) {
    raw[i] += prior[i];
  }
  for (uint32_t i = bpp; i < size; i++) {
    raw[i] += paeth_predictor(raw[i - bpp], prior[i], prior[i - bpp]);
  }
}



void unfilter(struct image *image) {
  // may be don't use a struct image
  
  uint8_t *data = image->data;
  uint32_t size = line_size(image);
  uint8_t  bpp  = (image->depth * image->sample + 7) / 8;
  LOG_TRACE("Byte Per Pixel %d", bpp);
  
  uint8_t *prior = NULL;
  uint8_t *raw = data + 1;
  
  for (uint32_t i = 0; i < image->height; i++) {
  
    LOG_TRACE("line %i   filter %d", i, raw[-1]);
    switch (raw[-1]) {
    case 0:
      break;
    case 1:
      sub_unfilter(size, raw, bpp);
      break;
    case 2:
      up_unfilter(size, raw, prior, bpp);
      break;
    case 3:
      average_unfilter(size, raw, prior, bpp);
      break;
    case 4:
      paeth_unfilter(size, raw, prior, bpp);
      break;
    default:
      LOG_FATAL("Unknown filter-byte %d", *raw);
      exit(1);
    }
    prior = raw;
    raw  += size + 1;
  }
  LOG_INFO("Unfilter done");
}
