#include "tetromino.hpp"

void Tetromino::render(const Application& app, const Grid& grid) const {
    for (const auto& mino : m_minos) {
        mino.render(app, grid);
    }
}
