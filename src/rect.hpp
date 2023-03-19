#pragma once

#include "point.hpp"

struct Rect final {
    Point top_left;
    Point bottom_right;

    constexpr Rect(Point top_left, Point bottom_right) : top_left{ top_left }, bottom_right{ bottom_right } { }
    constexpr Rect(int x, int y, int width, int height)
        : top_left{ x, y },
          bottom_right{ x + width - 1, y + height - 1 } { }
};
