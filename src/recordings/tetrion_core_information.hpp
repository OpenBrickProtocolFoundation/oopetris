
#pragma once

#include "game/mino_stack.hpp"

struct TetrionCoreInformation {
    u8 tetrion_index;
    u32 level;
    u64 score;
    u32 lines_cleared;
    MinoStack mino_stack;

    TetrionCoreInformation(
            u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
            u32 level,
            u64 score,
            u32 lines_cleared,
            MinoStack mino_stack
    )
        : tetrion_index{ tetrion_index },
          level{ level },
          score{ score },
          lines_cleared{ lines_cleared },
          mino_stack{ std::move(mino_stack) } {};
};
