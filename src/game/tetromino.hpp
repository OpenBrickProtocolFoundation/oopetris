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

struct Tetromino final {
private:
    using GridPoint = shapes::AbstractPoint<u8>;
    using ScreenCordsFunction = std::function<shapes::UPoint(const GridPoint&)>;

    GridPoint m_position;
    Rotation m_rotation{ Rotation::North };
    TetrominoType m_type;
    std::array<Mino, 4> m_minos;

public:
    using TetrominoPoint = shapes::AbstractPoint<u8>;
    using Pattern = std::array<TetrominoPoint, 4>;

    Tetromino(GridPoint position, TetrominoType type)
        : m_position{ position },
          m_type{ type },
          m_minos{ create_minos(position, m_rotation, type) } { }

    [[nodiscard]] TetrominoType type() const;
    [[nodiscard]] Rotation rotation() const;

    void render(
            const ServiceProvider& service_provider,
            MinoTransparency transparency,
            const double original_scale,
            const ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size
    ) const;

    void rotate_right();
    void rotate_left();
    void move_down();
    void move_up();
    void move_left();
    void move_right();
    void move(const shapes::AbstractPoint<i8> offset);

    [[nodiscard]] const std::array<Mino, 4>& minos() const;


private:
    void refresh_minos();

    static Pattern get_pattern(TetrominoType type, Rotation rotation) {
        return tetrominos.at(static_cast<usize>(type)).at(static_cast<usize>(rotation));
    }

    static std::array<Mino, 4> create_minos(GridPoint position, Rotation rotation, TetrominoType type) {
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
