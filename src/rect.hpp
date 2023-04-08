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
};


inline constexpr Rect operator+(Point lhs, Rect rhs) {
    return Rect{ lhs + rhs.top_left, lhs + rhs.bottom_right };
}

inline constexpr Rect operator+(Rect lhs, Point rhs) {
    return Rect{ lhs.top_left + rhs, lhs.bottom_right + rhs };
}