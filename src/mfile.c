
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"
#include "mfile.h"



const struct mfile map_file(const char *pathname) {
  LOG_INFO("Opening file %s", pathname);

  int fd = open(pathname, O_RDONLY);
  if (fd < 0) {
    LOG_FATAL("Can't open the file: %s", pathname);
    exit(1);
  }

  struct stat st;
  if (fstat(fd, &st) != 0) {
    LOG_FATAL("Can't get stats about the file: %s", pathname);
    close(fd);
    exit(1);
  }

  size_t file_size = (size_t) st.st_size;
  size_t page_size = getpagesize();
  // find the right size multiple of the page size containing the file
  size_t mult_size = ((file_size / page_size) + 1) * page_size;

  void * file_ptr = mmap(NULL, mult_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
  if (file_ptr == MAP_FAILED) {
    LOG_FATAL("Can't map the file [%s:%zd] in memory (asked size %zd)", pathname, file_size, mult_size);
    close(fd);
    exit(1);
  }
  LOG_ALLOC("Mmap %s (size %zu) in %p", pathname, file_size, file_ptr);
  
  if (close(fd) != 0) {
    LOG_ERROR("Can't close the file: %s", pathname);
  }

  const struct mfile res = {
    .pathname       = pathname,
    .data           = file_ptr,
    .size           = file_size,
    .allocated_size = mult_size,
  };
  return res;
}



void unmap_file(const struct mfile *file) {
  LOG_ALLOC("Unmap file %s, %p", file->pathname, file->data);
  if (munmap(file->data, file->allocated_size) != 0) {
    LOG_WARN("Can't munmap the file: %s (allocated size %zd)", file->pathname, file->allocated_size);
  }
}



int mfile_is_png(const struct mfile *file) {
  const int n = 8;
  uint8_t sig[] = {137, 80, 78, 71, 13, 10, 26, 10};
  
  if (file->size < n) {
    return 0;
  }

  for (int i = 0; i < n; i++) {
    if (sig[i] != ((uint8_t*) file->data)[i]) {
      LOG_WARN("file %s failed PNG signature", file->pathname);
      return 0;
    }
  }
  return 1;
}
