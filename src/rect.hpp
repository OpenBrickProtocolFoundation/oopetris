#pragma once

#include "point.hpp"

struct Rect final {
    Point top_left;
    Point bottom_right;

    constexpr Rect() = default;
    constexpr Rect(Point top_left, Point bottom_right) : top_left{ top_left }, bottom_right{ bottom_right } { }
    constexpr Rect(SDL_int x, SDL_int y, SDL_int width, SDL_int height)
        : top_left{ x, y },
          bottom_right{ static_cast<SDL_int>(x + width - 1), static_cast<SDL_int>(y + height - 1) } { }

    [[nodiscard]] constexpr SDL_int width() const {
        return bottom_right.x - top_left.x + 1;
    }

    [[nodiscard]] constexpr SDL_int height() const {
        return bottom_right.y - top_left.y + 1;
    }
};
