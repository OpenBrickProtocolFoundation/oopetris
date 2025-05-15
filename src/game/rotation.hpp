#pragma once

#include <core/helper/types.hpp>

#include "../helper/export_symbols.hpp"

enum class Rotation : u8 {
    North = 0,
    East,
    South,
    West,
    LastRotation = West,
};

OOPETRIS_GRAPHICS_EXPORTED Rotation& operator++(Rotation& rotation);

OOPETRIS_GRAPHICS_EXPORTED Rotation& operator--(Rotation& rotation);

OOPETRIS_GRAPHICS_EXPORTED Rotation operator+(Rotation rotation, i8 offset);

OOPETRIS_GRAPHICS_EXPORTED Rotation operator-(Rotation rotation, i8 offset);
