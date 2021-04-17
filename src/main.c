/**
 * @file main.c
 * @brief the main file
 * @details
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "chunk.h"
#include "image.h"
#include "log.h"
#include "mfile.h"
#include "print.h"
#include "viewer.h"


/** @brief max size of buffer */
#define BUFFER_SIZE (64U)
/** @brief Static buffer */
static char tmp_buffer[BUFFER_SIZE];



/**
 * @brief The main function
 * @param[in] argc Number of argument (> 0)
 * @param[in] argv Arguments as an array of string
 */
int main(int argc, char *argv[]) {
  LOG_LOG_LEVEL();
  const char *exec_name = argv[0];

  const char *file_name = NULL;
  const char *opt_param = NULL;
  enum command_option option = arg_parse(argc, argv, &opt_param, &file_name);
  LOG_INFO("Option %d   opt-param %s   file %s", option, opt_param, file_name);

  
  /*
   * Option that doesn't need file
   */
  
  switch (option) {

  case CMD_NONE:
    print_help(exec_name);
    return 0;
    
  case CMD_VERSION:
    print_version(exec_name);
    return 0;
    
  case CMD_HELP:
    print_help(exec_name);
    return 0;

  case CMD_ERROR:
    printf("wrong command line\n");
    return 1;

  case CMD_PLTE:
    printf("Not handled yet\n");
    return 0;

  default:; // go further
  }

  /*
   * Option that need a file
   */

  const struct mfile file = map_file(file_name);

  if (!mfile_is_png(&file)) {
    print_help(argv[0]);
    LOG_FATAL("%s is not a PNG", file.pathname);
    unmap_file(&file);
    return 1;
  }
  // file is PNG file
  
  switch (option) {

  case CMD_CHUNK:
    print_PNG_file(&file);
    break;

  case CMD_DISPLAY: {
    const struct image image = get_image(&file);
    view_image(&image);
    free_image(&image);
    break;
  }
    
  case CMD_BMP: {
    const struct image image = get_image(&file);
    save_image_as_bmp(&image, opt_param);
    free_image(&image);
    break;
  }

  case CMD_PASS: {

    // TODO check size!!!
    char *c = stpcpy(tmp_buffer, file_name) - 4;
    assert(*c == '.');
    char *nb = stpcpy(c, "(?).bmp") - 6;
    assert(*nb == '?');

    struct image pass[ADAM7_NB_PASS];
    get_adam7_passes(&file, pass);
    
    for (uint8_t p = 0; p < ADAM7_NB_PASS; p++) {
      *nb = '0' + p + 1; // char 'p + 1'
      save_image_as_bmp(pass + p, tmp_buffer);
    }
    free_passes(pass);
  }

  default:;
  }

  
  unmap_file(&file);
  LOG_INFO("\t Job done");
  return 0;
}
