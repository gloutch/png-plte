/**
 * @file print.h
 * @brief Provide functions to display texual info
 * @details Print function for an easier developpement
 */

#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdio.h>
#include <zlib.h>

#include "chunk.h"
#include "mfile.h"


/**
 * @brief Print version on stdout
 */
void print_version(void);

/**
 * @brief Print the chunk as one liner
 * @param[in] chunk
 */
void print_chunk(const struct chunk *chunk);

/**
 * @brief Print the whole file chunk by chunk
 * @param[in] file
 */
void print_PNG_file(const struct mfile *file);


#endif // __PRINT_H__
