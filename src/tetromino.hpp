#pragma once

#include "mino.hpp"
#include "point.hpp"
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

struct Application;
struct Grid;

struct Tetromino final {
private:
    Point m_position;
    Rotation m_rotation;
    TetrominoType m_type;
    std::array<Mino, 4> m_minos;

public:
    using Pattern = std::array<Point, 4>;

    Tetromino(Point position, TetrominoType type)
        : m_position{ position },
          m_rotation{ Rotation::North },
          m_type{ type },
          m_minos{ create_minos(position, m_rotation, type) } { }

    void render(const Application& app, const Grid& grid) const;

    void rotate_right() {
        ++m_rotation;
        refresh_minos();
    }

    void rotate_left() {
        --m_rotation;
        refresh_minos();
    }

    void move_down() {
        ++m_position.y;
        refresh_minos();
    }

    void move_up() {
        --m_position.y;
        refresh_minos();
    }

    void move_left() {
        --m_position.x;
        refresh_minos();
    }

    void move_right() {
        ++m_position.x;
        refresh_minos();
    }

    const std::array<Mino, 4>& minos() const {
        return m_minos;
    }

private:
    void refresh_minos() {
        m_minos = create_minos(m_position, m_rotation, m_type);
    }

    static constexpr Pattern get_pattern(TetrominoType type, Rotation rotation) {
        return tetrominos[static_cast<std::size_t>(type)][static_cast<std::size_t>(rotation)];
    }

    static constexpr std::array<Mino, 4> create_minos(Point position, Rotation rotation, TetrominoType type) {
        return std::array<Mino, 4>{
            Mino{position + get_pattern(type, rotation)[0], type},
            Mino{position + get_pattern(type, rotation)[1], type},
            Mino{position + get_pattern(type, rotation)[2], type},
            Mino{position + get_pattern(type, rotation)[3], type},
        };
    }

    using TetrominoPatterns = std::array<Pattern, 4>; // one pattern per rotation

    // clang-format off
    static constexpr std::array<TetrominoPatterns, 7> tetrominos = {
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
