
#include "viewer.h"

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

  uint32_t width  = image->width;
  uint32_t height = image->height;
  SDL_Window *window = SDL_CreateWindow(
    WINDOW_TITLE,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width,
    height,
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

  // black background
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

  // TODO fill the screen

  SDL_UpdateWindowSurface(window);
  wait_until_close();
  SDL_DestroyWindow(window);
  SDL_Quit();
}