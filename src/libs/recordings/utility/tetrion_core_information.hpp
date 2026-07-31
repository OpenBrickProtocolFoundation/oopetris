
#pragma once

#include <core/game/mino_stack.hpp>

struct TetrionCoreInformation {
    u8 tetrion_index;
    u32 level;
    u64 score;
    u32 lines_cleared;
    MinoStack mino_stack;

    TetrionCoreInformation(
            u8 tetrion_index_a, // NOLINT(bugprone-easily-swappable-parameters)
            u32 level_a,
            u64 score_a,
            u32 lines_cleared_a,
            MinoStack mino_stack_a
    )
        : tetrion_index{ tetrion_index_a },
          level{ level_a },
          score{ score_a },
          lines_cleared{ lines_cleared_a },
          mino_stack{ std::move(mino_stack_a) } { };
};
