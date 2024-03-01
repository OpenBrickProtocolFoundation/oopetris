
#pragma once

#include "game/mino_stack.hpp"

struct TetrionCoreInformation {
    u8 tetrion_index;
    u32 level;
    u64 score;
    u32 lines_cleared;
    MinoStack mino_stack;
};
