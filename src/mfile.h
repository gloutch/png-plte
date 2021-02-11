#ifndef MFILE_H
#define MFILE_H

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


/*
  This file contains function to map a entire file into memory

  Then you can access the file only using pointers.
*/



struct mfile {
  const char *pathname;
  uint8_t * ptr;
  size_t size;
  size_t real_size;
};

struct mfile map_mfile(const char *pathname);

void munmap_mfile(struct mfile *file);


// check for the 8-byte signature at the beginning of the file
int mfile_is_png(const struct mfile *file);



#endif // MFILE_H
