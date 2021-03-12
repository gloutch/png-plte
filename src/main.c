/**
 * @file main.c
 * @brief the main file
 * @details
 */


#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "image.h"
#include "log.h"
#include "mfile.h"
#include "print.h"
#include "viewer.h"


/**
 * @brief Prinf usage info of the tool
 * @param[in] argc Number of argument (> 0)
 * @param[in] argv Argument as an array of string from main
 */
void usage(int argc, char *argv[]) {
  printf("%s <PNG file>\n", argv[0]);
}


/**
 * @brief The main function
 * @param[in] argc Number of argument (> 0)
 * @param[in] argv Arguments as an array of string
 */
int main(int argc, char *argv[]) {
  LOG_LOG_LEVEL();
  print_version();

  if (argc < 2) {
    usage(argc, argv);
    exit(0);
  }

  const struct mfile file = map_file(argv[1]);

  if (!mfile_is_png(&file)) {
    usage(argc, argv);
    LOG_FATAL("%s is not a PNG", file.pathname);
    unmap_file(&file);
    exit(1);
  }

  // print_PNG_file(&file);

  const struct image image = image_from_png(&file);

  view_image(&image);
  
  free_image(&image);
  unmap_file(&file);
  
  LOG_INFO("\tDone: everything is fine ;D");
  return 0;
}
