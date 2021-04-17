#include <string.h>

#include "cli.h"
#include "log.h"


/**
 * @brief Short option for getopt
 * @details [getopt](https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html)
 */
static const char *short_option = "+";



enum command_option arg_parse(int argc, char *const *argv, const char **opt_param, const char **file) {

  if (argc <= 1) {
    LOG_TRACE("No argument");
    return CMD_HELP;
  }

  enum command_option option = CMD_NONE; // main option
  int opt_index = 0; // index of option in long_option
  
  // variable for getopt
  int index;
  int c;

  while ((c = getopt_long(argc, argv, short_option, long_option, &index)) != -1) {
    LOG_DEBUG("Parsing option -%c    long_index %d", c, index);
    
    if (option != CMD_NONE) {
      LOG_ERROR("Too many option --%s + -%c", long_option[opt_index].name, c);
      return CMD_ERROR;
    }
    
    switch (c) {
 
    case 'h':
      return CMD_HELP;
      
    case 'v':
      return CMD_VERSION;

    case 'p':
      option = CMD_PLTE;
      opt_index = index;
      break;

    case 'a':
      option = CMD_PASS;
      opt_index = index;
      break;
      
    case 'd':
      option = CMD_DISPLAY;
      opt_index = index;
      break;

    case 'c':
      option = CMD_CHUNK;
      opt_index = index;
      break;

    case 'b':
      LOG_TRACE("Option --bmp <%s>", optarg);
      if (strlen(optarg) == 0) {
        return CMD_ERROR;
      }
      option = CMD_BMP;
      opt_index = index;
      *opt_param = optarg;
      break;

    default:
      LOG_ERROR("Wrong option -%c", c);
      return CMD_ERROR;
    }
  }
  // here we need to get the filename
  
  if (optind >= argc) {
    LOG_ERROR("Missing filename requiered by --%s", long_option[opt_index].name);
    return CMD_ERROR;
  }
  if ((argc - optind) > 1) {
    LOG_ERROR("Too many files name");
    return CMD_ERROR;
  }
  // index + 1 == argc (only one argument)
  *file = argv[optind];
  return option;
}
