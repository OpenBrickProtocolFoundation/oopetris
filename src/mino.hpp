#pragma once

#include "application.hpp"
#include "grid.hpp"
#include "point.hpp"
#include "tetromino_type.hpp"

struct Mino final {
private:
    Point coords;
    TetrominoType type;
    static constexpr int inset = 3;

public:
    Mino(Point coords, TetrominoType type) : coords{ coords }, type{ type } { }

    void render(const Application& app, const Grid& grid) const;
};
