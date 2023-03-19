#pragma once

struct Point final {
    int x;
    int y;

    constexpr Point() : Point{ 0, 0 } { }
    constexpr Point(int x, int y) : x{ x }, y{ y } { }

    static constexpr Point zero() {
        return Point{};
    }
};

inline constexpr bool operator==(Point lhs, Point rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline constexpr bool operator!=(Point lhs, Point rhs) {
    return !(lhs == rhs);
}

inline constexpr Point operator*(Point point, int scale) {
    return Point{ point.x * scale, point.y * scale };
}

inline constexpr Point operator*(int scale, Point point) {
    return point * scale;
}

inline constexpr Point operator/(Point point, int divisor) {
    return Point{ point.x / divisor, point.y / divisor };
}

inline constexpr Point operator+(Point lhs, Point rhs) {
    return Point{ lhs.x + rhs.x, lhs.y + rhs.y };
}

inline constexpr Point operator+(Point point) {
    return point;
}

inline constexpr Point operator-(Point point) {
    return Point{ -point.x, -point.y };
}

inline constexpr Point operator-(Point lhs, Point rhs) {
    return lhs + (-rhs);
}
