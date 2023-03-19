#pragma once

#include "point.hpp"
#include "tetromino_type.hpp"
#include <array>

enum class Rotation {
    North = 0,
    East,
    South,
    West,
};

struct Tetromino final {
    using Pattern = std::array<Point, 4>;
    using TetrominoPatterns = std::array<Pattern, 4>; // one pattern per rotation

    static constexpr Pattern get_pattern(TetrominoType type, Rotation rotation) {
        return tetrominos[static_cast<std::size_t>(type)][static_cast<std::size_t>(rotation)];
    }

private:
    // clang-format off
    static constexpr std::array<TetrominoPatterns, 7> tetrominos = {
        // I
        TetrominoPatterns{
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 3, 1 }, },
            Pattern{ Point{ 2, 0 }, Point{ 2, 1 }, Point{ 2, 2 }, Point{ 2, 3 }, },
            Pattern{ Point{ 0, 2 }, Point{ 1, 2 }, Point{ 2, 2 }, Point{ 3, 2 }, },
            Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, Point{ 1, 3 }, },
        },
        // J
        TetrominoPatterns{
            Pattern{ Point{ 0, 0 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 2, 0 }, Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, },
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 2, 2 }, },
            Pattern{ Point{ 0, 2 }, Point{ 1, 2 }, Point{ 1, 1 }, Point{ 1, 0 }, },
        },
        // L
        TetrominoPatterns{
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 2, 0 }, },
            Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, Point{ 2, 2 }, },
            Pattern{ Point{ 0, 2 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 0, 0 }, Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, },
        },
        // O
        TetrominoPatterns{
            Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
        },
        // S
        TetrominoPatterns{
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 0 }, Point{ 2, 0 }, },
            Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 2, 2 }, },
            Pattern{ Point{ 0, 2 }, Point{ 1, 2 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 0, 0 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 2 }, },
        },
        // T
        TetrominoPatterns{
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 0 }, Point{ 2, 0 }, },
            Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 1, 2 }, },
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 1, 2 }, },
            Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 0, 1 }, Point{ 1, 2 }, },
        },
        // Z
        TetrominoPatterns{
            Pattern{ Point{ 0, 0 }, Point{ 1, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
            Pattern{ Point{ 2, 0 }, Point{ 2, 1 }, Point{ 1, 1 }, Point{ 1, 2 }, },
            Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 2 }, Point{ 2, 2 }, },
            Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 0, 1 }, Point{ 0, 2 }, },
        },
    };
    // clang-format on
};
