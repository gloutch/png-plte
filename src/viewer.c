
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



void view_image(const struct image *image) {

  int err = SDL_Init(SDL_INIT_VIDEO);
  if (err != 0) {
    LOG_FATAL("Can't init SDL, returned %d", err);
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
    LOG_FATAL("Can't create a window");
    exit(1);
  }

  SDL_Surface *screen = SDL_GetWindowSurface(window);
  if (screen == NULL) {
    LOG_FATAL("Can't get the window surface");
    exit(1);
  }

  // Set background color
  SDL_FillRect(screen, NULL, SDL_MapRGB(
    screen->format,
    default_bg_color[0],
    default_bg_color[1],
    default_bg_color[2]));

  for (uint32_t i = 0; i < image->height; i++) {
    for (uint32_t j = 0; j < image->width; j++) {

      struct color png_color;
      get_color(image, i, j, &png_color);

      // [0, max] -> [0, 255]
      uint8_t red   = png_color.red   * 255.0 / png_color.max;
      uint8_t green = png_color.green * 255.0 / png_color.max;
      uint8_t blue  = png_color.blue  * 255.0 / png_color.max;
      
      // Transparency ratio [0:transparent, 1:opaque]
      float a = (float) png_color.alpha / (float) png_color.max;

      // Apply ransparency
      red   = red   * a  + default_bg_color[0] * (1.0 - a);
      green = green * a  + default_bg_color[1] * (1.0 - a);
      blue  = blue  * a  + default_bg_color[2] * (1.0 - a);
      
      uint32_t sdl_color = SDL_MapRGB(screen->format, red, green, blue);
      ((uint32_t *) screen->pixels)[i * image->width + j] = sdl_color;
    }
  }

  SDL_UpdateWindowSurface(window);
  wait_until_close();
  SDL_DestroyWindow(window);
  SDL_Quit();
}
