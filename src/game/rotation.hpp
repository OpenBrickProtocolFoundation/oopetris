#pragma once

#include "helper/types.hpp"

enum class Rotation : u8 {
    North = 0,
    East,
    South,
    West,
    LastRotation = West,
};

Rotation& operator++(Rotation& rotation);

Rotation& operator--(Rotation& rotation);

Rotation operator+(Rotation rotation, i8 offset);

Rotation operator-(const Rotation rotation, i8 offset);
