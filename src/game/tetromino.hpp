#pragma once

#include "graphics/point.hpp"
#include "mino.hpp"
#include "tetromino_type.hpp"
#include <array>

enum class Rotation {
    North = 0,
    East,
    South,
    West,
    LastRotation = West,
};

inline Rotation& operator++(Rotation& rotation) {
    rotation = static_cast<Rotation>((static_cast<int>(rotation) + 1) % (static_cast<int>(Rotation::LastRotation) + 1));
    return rotation;
}

inline Rotation& operator--(Rotation& rotation) {
    rotation = static_cast<Rotation>(
            (static_cast<int>(rotation) + static_cast<int>(Rotation::LastRotation))
            % (static_cast<int>(Rotation::LastRotation) + 1)
    );
    return rotation;
}

inline Rotation operator+(Rotation rotation, const int offset) {
    if (offset == 0) {
        return rotation;
    }

    if (offset > 0) {
        for (usize i = 0; i < static_cast<usize>(offset); ++i) {
            ++rotation;
        }
        return rotation;
    }

    for (usize i = 0; i < static_cast<usize>(-offset); ++i) {
        --rotation;
    }
    return rotation;
}

inline Rotation operator-(const Rotation rotation, const int offset) {
    return rotation + (-offset);
}

struct Grid;

struct Tetromino final {
private:
    Point m_position;
    Rotation m_rotation{ Rotation::North };
    TetrominoType m_type;
    std::array<Mino, 4> m_minos;

public:
    using Pattern = std::array<Point, 4>;

    Tetromino(Point position, TetrominoType type)
        : m_position{ position },
          m_type{ type },
          m_minos{ create_minos(position, m_rotation, type) } { }

    [[nodiscard]] TetrominoType type() const {
        return m_type;
    }

    [[nodiscard]] Rotation rotation() const {
        return m_rotation;
    }

    void render(
            const ServiceProvider& service_provider,
            const Grid* grid,
            const MinoTransparency transparency,
            const Point& offset = Point::zero()
    ) const {
        for (const auto& mino : m_minos) {
            mino.render(service_provider, grid, transparency, offset);
        }
    }

    void rotate_right() {
        ++m_rotation;
        refresh_minos();
    }

    void rotate_left() {
        --m_rotation;
        refresh_minos();
    }

    void move_down() {
        move({ 0, 1 });
    }

    void move_up() {
        move({ 0, -1 });
    }

    void move_left() {
        move({ -1, 0 });
    }

    void move_right() {
        move({ 1, 0 });
    }

    void move(const Point offset) {
        m_position += offset;
        refresh_minos();
    }

    [[nodiscard]] const std::array<Mino, 4>& minos() const {
        return m_minos;
    }

private:
    void refresh_minos() {
        m_minos = create_minos(m_position, m_rotation, m_type);
    }

    static Pattern get_pattern(TetrominoType type, Rotation rotation) {
        return tetrominos.at(static_cast<usize>(type)).at(static_cast<usize>(rotation));
    }

    static std::array<Mino, 4> create_minos(Point position, Rotation rotation, TetrominoType type) {
        return std::array<Mino, 4>{
            Mino{position + get_pattern(type, rotation).at(0), type},
            Mino{position + get_pattern(type, rotation).at(1), type},
            Mino{position + get_pattern(type, rotation).at(2), type},
            Mino{position + get_pattern(type, rotation).at(3), type},
        };
    }

    using TetrominoPatterns = std::array<Pattern, 4>; // one pattern per rotation

    // clang-format off
    static constexpr auto tetrominos = std::array<Tetromino::TetrominoPatterns, 7>{
        // I
        TetrominoPatterns{
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 3, 1 }, },
                          Pattern{ Point{ 2, 0 }, Point{ 2, 1 }, Point{ 2, 2 }, Point{ 2, 3 }, },
                          Pattern{ Point{ 0, 2 }, Point{ 1, 2 }, Point{ 2, 2 }, Point{ 3, 2 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, Point{ 1, 3 }, },
                          },
        // J
        TetrominoPatterns{
                          Pattern{ Point{ 0, 0 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 2, 0 }, Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, },
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 2, 2 }, },
                          Pattern{ Point{ 0, 2 }, Point{ 1, 2 }, Point{ 1, 1 }, Point{ 1, 0 }, },
                          },
        // L
        TetrominoPatterns{
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 2, 0 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, Point{ 2, 2 }, },
                          Pattern{ Point{ 0, 2 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 0, 0 }, Point{ 1, 0 }, Point{ 1, 1 }, Point{ 1, 2 }, },
                          },
        // O
        TetrominoPatterns{
                          Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 2, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          },
        // S
        TetrominoPatterns{
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 0 }, Point{ 2, 0 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 2, 2 }, },
                          Pattern{ Point{ 0, 2 }, Point{ 1, 2 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 0, 0 }, Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 2 }, },
                          },
        // T
        TetrominoPatterns{
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 0 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 1, 2 }, },
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 }, Point{ 1, 2 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 0, 1 }, Point{ 1, 2 }, },
                          },
        // Z
        TetrominoPatterns{
                          Pattern{ Point{ 0, 0 }, Point{ 1, 0 }, Point{ 1, 1 }, Point{ 2, 1 }, },
                          Pattern{ Point{ 2, 0 }, Point{ 2, 1 }, Point{ 1, 1 }, Point{ 1, 2 }, },
                          Pattern{ Point{ 0, 1 }, Point{ 1, 1 }, Point{ 1, 2 }, Point{ 2, 2 }, },
                          Pattern{ Point{ 1, 0 }, Point{ 1, 1 }, Point{ 0, 1 }, Point{ 0, 2 }, },
                          },
    };
    // clang-format on
};
