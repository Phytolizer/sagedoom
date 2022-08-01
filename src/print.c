#include "doom/print.h"

#include <sys/ioctl.h>
#include <unistd.h>

static size_t l_get_terminal_width(void) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return 80;
  }
  return ws.ws_col;
}

void print_centered(str text, char pad) {
  static size_t size = 0;
  if (size == 0) {
    size = l_get_terminal_width();
  }
  size_t pad_left = (size - str_len(text)) / 2;
  size_t pad_right = size - str_len(text) - pad_left;
  for (size_t i = 0; i < pad_left; ++i) {
    putchar(pad);
  }
  printf("%s", str_ptr(text));
  for (size_t i = 0; i < pad_right; ++i) {
    putchar(pad);
  }
  putchar('\n');
}
