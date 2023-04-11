#pragma once

#include "mino.hpp"
#include <vector>

struct Grid;

struct MinoStack final {
private:
    std::vector<Mino> m_minos;

public:
    void clear_row_and_let_sink(int row);
    [[nodiscard]] bool is_empty(Point coordinates) const;
    void set(Point coordinates, TetrominoType type);
    void draw_minos(const Application& app, const Grid& grid) const;
};
