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
    LastType = Z,
};

constexpr Color get_foreground_color(TetrominoType type, std::uint8_t alpha = 255);
constexpr Color get_background_color(TetrominoType type, std::uint8_t alpha = 255);
