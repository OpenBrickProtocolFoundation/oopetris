#pragma once

#include "color.hpp"

enum class TetrominoType {
    I = 0,
    J,
    L,
    O,
    S,
    T,
    Z,
};

Color get_foreground_color(TetrominoType type);
Color get_background_color(TetrominoType type);
