/**
 * @file main.c
 * @brief the main file
 * @details
 */


#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "mfile.h"


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

  if (argc < 2) {
    usage(argc, argv);
    exit(0);
  }

  const struct mfile file = map_file(argv[1]);

  if (!mfile_is_png(&file)) {
    LOG_FATAL("%s is not a PNG", file.pathname);
    unmap_file(&file);
    exit(1);
  }

  unmap_file(&file);
  return 0;
}
