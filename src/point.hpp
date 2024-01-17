#pragma once

#include "sdl_compatibility.h"

struct Point final {
    SDL_int x;
    SDL_int y;

    constexpr Point() : Point{ 0, 0 } { }
    constexpr Point(SDL_int x, SDL_int y) : x{ x }, y{ y } { }

    static constexpr Point zero() {
        return Point{};
    }
};

inline constexpr bool operator==(Point lhs, Point rhs) {
    return lhs.x == rhs.x and lhs.y == rhs.y;
}

inline constexpr bool operator!=(Point lhs, Point rhs) {
    return not(lhs == rhs);
}

inline constexpr Point operator*(Point point, SDL_int scale) {
    return Point{ static_cast<SDL_int>(point.x * scale), static_cast<SDL_int>(point.y * scale) };
}

inline constexpr Point operator*(int scale, Point point) {
    return point * scale;
}

inline constexpr Point operator/(Point point, SDL_int divisor) {
    return Point{ static_cast<SDL_int>(point.x / divisor), static_cast<SDL_int>(point.y / divisor) };
}

inline constexpr Point operator+(Point lhs, Point rhs) {
    return Point{ static_cast<SDL_int>(lhs.x + rhs.x), static_cast<SDL_int>(lhs.y + rhs.y) };
}

inline constexpr Point operator+(Point point) {
    return point;
}

inline constexpr Point operator-(Point point) {
    return Point{ static_cast<SDL_int>(-point.x), static_cast<SDL_int>(-point.y) };
}

inline constexpr Point operator-(Point lhs, Point rhs) {
    return lhs + (-rhs);
}

inline constexpr Point operator+=(Point& lhs, Point rhs) {
    lhs = lhs + rhs;
    return lhs;
}

inline constexpr Point operator-=(Point& lhs, Point rhs) {
    lhs = lhs - rhs;
    return lhs;
}
