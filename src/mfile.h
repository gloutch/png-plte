/**
 * @file mfile.h
 * @brief In memory file
 * @details In order to go through a whole file easily, a struct mfile is a entire file mapped in memory.
 * Then access the file only using pointers.
 */

#ifndef __MFILE_H__
#define __MFILE_H__

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"


/**
 * @brief Handle some data about mapped file
 */
struct mfile {
  /** @brief Pointer to the path to the mapped file */
  const char *pathname;
  /** @brief Pointer to the beginning of the mapped file */
  void *data;
  /** @brief File size */
  size_t file_size;
  /** @brief Allocated size */
  size_t allocated_size;
};


/**
 * @brief Map a file to the memory
 * @param[in] pathname Path to the file to open
 * @return the allocated file
 */
const struct mfile map_file(const char *pathname);

/**
 * @brief Unmap a mfile from allocated memory
 * @param[in] file The allocated file to free
 */
void unmap_file(const struct mfile *file);

/**
 * @brief Check for the 8-byte signature at the beginning of the file
 * @details The PNG signature is 8 bytes described in 
 * [PNG-file-signature](http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html#PNG-file-signature)
 * @param[in] file The allocated file to check in
 * @return 0 is file is not a PNG, 1 otherwise
 */
int mfile_is_png(const struct mfile *file);



#endif // __MFILE_H__
