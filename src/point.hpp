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

inline bool operator==(Point lhs, Point rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(Point lhs, Point rhs) {
    return !(lhs == rhs);
}

inline Point operator*(Point point, int scale) {
    return Point{ point.x * scale, point.y * scale };
}

inline Point operator*(int scale, Point point) {
    return point * scale;
}

inline Point operator/(Point point, int divisor) {
    return Point{ point.x / divisor, point.y / divisor };
}

inline Point operator+(Point lhs, Point rhs) {
    return Point{ lhs.x + rhs.x, lhs.y + rhs.y };
}

inline Point operator+(Point point) {
    return point;
}

inline Point operator-(Point point) {
    return Point{ -point.x, -point.y };
}

inline Point operator-(Point lhs, Point rhs) {
    return lhs + (-rhs);
}
