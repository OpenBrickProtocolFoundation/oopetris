#pragma once

#include "point.hpp"

struct Rect final {
    Point top_left;
    Point bottom_right;

    constexpr Rect() = default;
    constexpr Rect(Point top_left, Point bottom_right) : top_left{ top_left }, bottom_right{ bottom_right } { }
    constexpr Rect(int x, int y, int width, int height)
        : top_left{ x, y },
          bottom_right{ x + width - 1, y + height - 1 } { }

    [[nodiscard]] constexpr auto width() const {
        return bottom_right.x - top_left.x + 1;
    }

    [[nodiscard]] constexpr auto height() const {
        return bottom_right.y - top_left.y + 1;
    }
};
