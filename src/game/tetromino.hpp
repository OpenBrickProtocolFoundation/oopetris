#pragma once

#include "graphics/point.hpp"
#include "mino.hpp"
#include "rotation.hpp"
#include "tetromino_type.hpp"

#include <array>


struct Tetromino final {
private:
    using GridPoint = shapes::AbstractPoint<u8>;
    using ScreenCordsFunction = std::function<shapes::UPoint(const GridPoint&)>;

    GridPoint m_position;
    Rotation m_rotation{ Rotation::North };
    helper::TetrominoType m_type;
    std::array<Mino, 4> m_minos;

public:
    using TetrominoPoint = shapes::AbstractPoint<u8>;
    using Pattern = std::array<TetrominoPoint, 4>;

    Tetromino(GridPoint position, helper::TetrominoType type)
        : m_position{ position },
          m_type{ type },
          m_minos{ create_minos(position, m_rotation, type) } { }

    [[nodiscard]] helper::TetrominoType type() const;
    [[nodiscard]] Rotation rotation() const;

    void render(
            const ServiceProvider& service_provider,
            MinoTransparency transparency,
            double original_scale,
            const ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size,
            const GridPoint& offset = GridPoint::zero()
    ) const;

    void rotate_right();
    void rotate_left();
    void move_down();
    void move_up();
    void move_left();
    void move_right();
    void move(shapes::AbstractPoint<i8> offset);

    [[nodiscard]] const std::array<Mino, 4>& minos() const;


private:
    void refresh_minos();

    static Pattern get_pattern(helper::TetrominoType type, Rotation rotation);

    static std::array<Mino, 4> create_minos(GridPoint position, Rotation rotation, helper::TetrominoType type);

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
