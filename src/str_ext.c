#include "str.h"

bool str_has_suffix_ci(str s, str suffix) {
  if (str_len(s) < str_len(suffix)) {
    return false;
  }

  str s_end = str_ref(str_end(s) - str_len(suffix));
  return str_eq_ci(s_end, suffix);
}
