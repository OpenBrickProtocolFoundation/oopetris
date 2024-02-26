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
    shapes::UPoint m_position;
    Rotation m_rotation{ Rotation::North };
    TetrominoType m_type;
    std::array<Mino, 4> m_minos;

public:
    using TetrominoPoint = shapes::AbstractPoint<u8>;
    using Pattern = std::array<TetrominoPoint, 4>;

    Tetromino(shapes::UPoint position, TetrominoType type)
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
            const shapes::UPoint& offset = shapes::UPoint::zero()
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

    void move(const shapes::AbstractPoint<i8> offset) {
        // this looks weird but silently asserts, that the final point is not negative
        m_position = (m_position.cast<i32>() + offset).cast<u32>();
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

    static std::array<Mino, 4> create_minos(shapes::UPoint position, Rotation rotation, TetrominoType type) {
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
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 3, 1 }, },
                          Pattern{ TetrominoPoint{ 2, 0 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 2, 2 }, TetrominoPoint{ 2, 3 }, },
                          Pattern{ TetrominoPoint{ 0, 2 }, TetrominoPoint{ 1, 2 }, TetrominoPoint{ 2, 2 }, TetrominoPoint{ 3, 2 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, TetrominoPoint{ 1, 3 }, },
                          },
        // J
        TetrominoPatterns{
                          Pattern{ TetrominoPoint{ 0, 0 }, TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 2, 0 }, TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, },
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 2, 2 }, },
                          Pattern{ TetrominoPoint{ 0, 2 }, TetrominoPoint{ 1, 2 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 0 }, },
                          },
        // L
        TetrominoPatterns{
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 2, 0 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, TetrominoPoint{ 2, 2 }, },
                          Pattern{ TetrominoPoint{ 0, 2 }, TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 0, 0 }, TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, },
                          },
        // O
        TetrominoPatterns{
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 2, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 2, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 2, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 2, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          },
        // S
        TetrominoPatterns{
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 0 }, TetrominoPoint{ 2, 0 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 2, 2 }, },
                          Pattern{ TetrominoPoint{ 0, 2 }, TetrominoPoint{ 1, 2 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 0, 0 }, TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, },
                          },
        // T
        TetrominoPatterns{
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 0 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 1, 2 }, },
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 1, 2 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 2 }, },
                          },
        // Z
        TetrominoPatterns{
                          Pattern{ TetrominoPoint{ 0, 0 }, TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 2, 1 }, },
                          Pattern{ TetrominoPoint{ 2, 0 }, TetrominoPoint{ 2, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, },
                          Pattern{ TetrominoPoint{ 0, 1 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 1, 2 }, TetrominoPoint{ 2, 2 }, },
                          Pattern{ TetrominoPoint{ 1, 0 }, TetrominoPoint{ 1, 1 }, TetrominoPoint{ 0, 1 }, TetrominoPoint{ 0, 2 }, },
                          },
    };
    // clang-format on
};
