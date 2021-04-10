/**
 * @file print.h
 * @brief Provide functions to display texual info
 * @details Print function for an easier developpement
 */

#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdio.h>
#include <string.h>

#include "chunk.h"
#include "mfile.h"


/**
 * @brief Print version on stdout
 * @param[in] exec Name of the executable
 */
void print_version(const char *exec);

/**
 * @brief Print the CLI help
 * @param[in] exec Name of the executable
 */
void print_help(const char *exec);

/**
 * @brief Print the chunk as one liner
 * @param[in] chunk
 * @param[in] header Needed to print some chunk (NULL is fine if you are sure header isn't needed)
 */
void print_chunk(const struct chunk *chunk, const struct IHDR *header);

/**
 * @brief Print the whole file chunk by chunk
 * @param[in] file
 */
void print_PNG_file(const struct mfile *file);


#endif // __PRINT_H__
