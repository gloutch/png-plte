
#include "mfile.h"


struct mfile map_mfile(const char *pathname) {
  // printf("file %s\n", pathname);

  int fd = open(pathname, O_RDONLY);
  if (fd < 0) {
    perror("Can't open the file");
    exit(0);
  }

  struct stat st;
  if (fstat(fd, &st) != 0) {
    perror("Can't obtain stats about the file");
    close(fd);
    exit(0);
  }

  size_t file_size = (size_t) st.st_size;
  size_t page_size = getpagesize();
  // find the right size multiple of the page size containing the file
  size_t mult_size = ((file_size / page_size) + 1) * page_size;

  void * file_ptr = mmap(NULL, mult_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
  if (file_ptr == MAP_FAILED) {
    perror("Can't map the file in memory");
    close(fd);
    exit(0);
  }

  if (close(fd) != 0) {
    perror("Can't close the file");
    exit(0);
  }

  struct mfile res;
  res.pathname  = pathname;
  res.ptr       = file_ptr;
  res.size      = file_size;
  res.real_size = mult_size;

  return res;
}

void munmap_mfile(struct mfile *file) {
  if (munmap(file->ptr, file->real_size) != 0) {
    perror("Can't unmap the file");
    exit(0);
  }
  file->ptr = NULL;
}



int mfile_is_png(const struct mfile *file) {

  // http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html#PNG-file-signature
  const int n = 8;
  uint8_t sig[] = {137, 80, 78, 71, 13, 10, 26, 10};

  for (int i = 0; i < n; i++) {
    if (sig[i] != (file->ptr)[i]) {
      return 0;
    }		
  }
  return 1;
}
