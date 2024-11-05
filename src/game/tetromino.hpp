#pragma once

#include <core/core.hpp>

#include "graphic_helpers.hpp"
#include "helper/windows.hpp"
#include "rotation.hpp"

#include <array>


struct Tetromino final {
private:
    using ScreenCordsFunction = Mino::ScreenCordsFunction;

    grid::GridPoint m_position;
    Rotation m_rotation{ Rotation::North };
    helper::TetrominoType m_type;
    std::array<Mino, 4> m_minos;

public:
    using TetrominoPoint = shapes::AbstractPoint<i8>;
    using Pattern = std::array<TetrominoPoint, 4>;

    OOPETRIS_GRAPHICS_EXPORTED Tetromino(grid::GridPoint position, helper::TetrominoType type)
        : m_position{ position },
          m_type{ type },
          m_minos{ create_minos(position, m_rotation, type) } { }

    OOPETRIS_GRAPHICS_EXPORTED Tetromino(grid::GridUPoint position, helper::TetrominoType type)
        : m_position{ position.cast<i8>() },
          m_type{ type },
          m_minos{ create_minos(m_position, m_rotation, type) } { }

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::TetrominoType type() const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Rotation rotation() const;

    OOPETRIS_GRAPHICS_EXPORTED void render(
            const ServiceProvider& service_provider,
            MinoTransparency transparency,
            double original_scale,
            const ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size,
            const grid::GridUPoint& offset = grid::GridUPoint::zero()
    ) const;

    OOPETRIS_GRAPHICS_EXPORTED void rotate_right();
    OOPETRIS_GRAPHICS_EXPORTED void rotate_left();
    OOPETRIS_GRAPHICS_EXPORTED void move_down();
    OOPETRIS_GRAPHICS_EXPORTED void move_up();
    OOPETRIS_GRAPHICS_EXPORTED void move_left();
    OOPETRIS_GRAPHICS_EXPORTED void move_right();
    OOPETRIS_GRAPHICS_EXPORTED void move(shapes::AbstractPoint<i8> offset);

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const std::array<Mino, 4>& minos() const;


private:
    void refresh_minos();

    static Pattern get_pattern(helper::TetrominoType type, Rotation rotation);

    static std::array<Mino, 4> create_minos(grid::GridPoint position, Rotation rotation, helper::TetrominoType type);

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
