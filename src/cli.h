/**
 * @file cli.h
 * @brief Client Line Interface
 * @details
 */


#ifndef __CLI_H__
#define __CLI_H__

#include <getopt.h>


/**
 * @brief Command option from the cli
 */
enum command_option {
  /** @brief Bad command line */
  CMD_ERROR = 0,
  /** @brief No option */
  CMD_NONE = 1,
  /** @brief Help command */
  CMD_HELP = 2,
  /** @brief Version */
  CMD_VERSION = 3,
  /** @brief Print chunks in a file */
  CMD_CHUNK = 4,
  /** @brief Display the PNG file */
  CMD_DISPLAY = 5,
  /** @brief Save the image to BMP */
  CMD_BMP = 6,
  /** @brief Get the palette */
  CMD_PLTE = 7,
};

/**
 * @brief Long option for getopt
 * @details [getopt_long](https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html)
 */
static const struct option long_option[] = {
  {"help",    no_argument,       NULL, 'h'},
  {"version", no_argument,       NULL, 'v'},
  {"chunk",   no_argument,       NULL, 'c'},
  {"display", no_argument,       NULL, 'd'},
  {"bmp",     required_argument, NULL, 'b'},
  {"plte",    no_argument,       NULL, 'p'},
  {NULL,      0,                 NULL,  0 },
};


/**
 * @brief Parse the argument from main() and find out what to do
 * @param[in] argc Length of argv
 * @param[in,out] argv Array of strings from main
 * @param[out] opt Pointer to the string argument (NULL if none)
 * @param[out] file Pointer to the arrays holding each file name (NULL if none)
 * @return The core option of the command
 */
enum command_option arg_parse(int argc, char *const *argv, const char **opt, const char **file);


#endif // __CLI_H__
