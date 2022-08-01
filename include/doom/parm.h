#pragma once

#include "doom/state.h"
#include "util/maybe.h"

#include <stdbool.h>
#include <str.h>

bool parm_exists(DoomState* state, str parm);
MaybeSize parm_index(DoomState* state, str parm);
