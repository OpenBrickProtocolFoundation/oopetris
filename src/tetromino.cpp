#include "tetromino.hpp"

void Tetromino::render(const Application& app, const Grid& grid) const {
    for (const auto& mino : m_minos) {
        mino.render(app, grid);
    }
}

// clang-format off
std::array<Tetromino::TetrominoPatterns, 7> Tetromino::tetrominos{
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
                      Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 0 }, Point{ 2, 1 }, },
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
