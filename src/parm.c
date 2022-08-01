#include "doom/parm.h"

bool parm_exists(DoomState* state, str parm) {
  for (size_t i = 0; i < state->argc; ++i) {
    if (str_eq(state->argv[i], parm)) {
      return true;
    }
  }
  return false;
}

MaybeSize parm_index(DoomState* state, str parm) {
  for (size_t i = 0; i < state->argc; ++i) {
    if (str_eq(state->argv[i], parm)) {
      return MAYBE_SOME(MaybeSize, i);
    }
  }
  return MAYBE_NONE(MaybeSize);
}
