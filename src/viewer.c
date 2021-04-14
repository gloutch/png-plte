#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "color.h"
#include "log.h"
#include "viewer.h"


/**
 * @brief Tilte of the window
 */
#define WINDOW_TITLE "PNG viewer"

/**
 * @brief Sleeping time before polling event (ms)
 */
#define DELAY (100)


/**
 * @brief Block until event SDL_QUIT
 */
static void wait_until_close(void) {

  SDL_Event event;
  uint8_t live = 1;

  while (live) {

    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
      live = 0;
    }
    SDL_Delay(DELAY);
  }
}

/**
 * @brief Copy image on an SDL_Surface
 * @param[in] image
 * @param[in,out] suface
 */
static void image_on_surface(const struct image *image, SDL_Surface *surface) {
  assert(image->width  == surface->w);
  assert(image->height == surface->h);
  
  struct color png_color;

  for (uint32_t i = 0; i < image->height; i++) {
    for (uint32_t j = 0; j < image->width; j++) {

      get_color(image, i, j, &png_color);
        
      // png color [0, max] -> sdl [0, 255]
      uint8_t red   = png_color.red   * 255.0 / png_color.max;
      uint8_t green = png_color.green * 255.0 / png_color.max;
      uint8_t blue  = png_color.blue  * 255.0 / png_color.max;
      
      // Transparency ratio [0:transparent, 1:opaque]
      float a = (float) png_color.alpha / (float) png_color.max;

      // Apply ransparency
      red   = red   * a  + default_bg_color.r * (1.0 - a);
      green = green * a  + default_bg_color.g * (1.0 - a);
      blue  = blue  * a  + default_bg_color.b * (1.0 - a);
      
      uint32_t sdl_color = SDL_MapRGB(surface->format, red, green, blue);
      ((uint32_t *) surface->pixels)[i * image->width + j] = sdl_color;
    }
  }
}



void view_image(const struct image *image) {

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    LOG_FATAL("Can't init SDL: %s", SDL_GetError());
    exit(1);
  }

  SDL_Window *window = SDL_CreateWindow(
    WINDOW_TITLE,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    image->width,
    image->height,
    SDL_WINDOW_SHOWN); // SDL_WINDOW_BORDERLESS

  if (window == NULL) {
    LOG_FATAL("Can't create a window: %s", SDL_GetError());
    exit(1);
  }

  SDL_Surface *screen = SDL_GetWindowSurface(window);
  if (screen == NULL) {
    LOG_FATAL("Can't get the window surface: %s", SDL_GetError());
    exit(1);
  }

  image_on_surface(image, screen);

  SDL_UpdateWindowSurface(window);
  wait_until_close();
  SDL_DestroyWindow(window);
  SDL_Quit();
}


void save_image_as_bmp(const struct image *image, const char *filename) {

  // create surface with default mask and depth
  SDL_Surface *surface = SDL_CreateRGBSurface(0, image->width, image->height, 32, 0, 0, 0, 0);
  if (surface == NULL) {
    LOG_FATAL("Can't create the SDL_Surface: %s", SDL_GetError());
    exit(1);
  }

  image_on_surface(image, surface);

  if (SDL_SaveBMP(surface, filename) != 0) {
    LOG_FATAL("Can't save surface as BMP: %s", SDL_GetError());
    exit(1);
  }
  LOG_INFO("Write %s", filename);
}
