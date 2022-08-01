#include "doom/file_util.h"

#include "doom/error.h"

#include <sys/stat.h>

off_t file_length(FILE* f) {
  int fd = fileno(f);
  struct stat statbuf;
  if (fstat(fd, &statbuf) == -1) {
    doom_error("fstat failed");
  }

  return statbuf.st_size;
}
