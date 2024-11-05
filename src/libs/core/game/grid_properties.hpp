

#pragma once

#include "../helper/point.hpp"

namespace grid {

    using GridType = i8;
    using GridPoint = shapes::AbstractPoint<GridType>;

    using GridUType = u8;
    using GridUPoint = shapes::AbstractPoint<GridUType>;

    constexpr GridUType original_tile_size = 26;

    constexpr GridUType width_in_tiles = 10;
    constexpr GridUType height_in_tiles = 20;

    constexpr GridUPoint preview_extends{ 4, (6 * 3) + 1 };
    constexpr GridUPoint hold_background_position{ 0, 0 };
    constexpr GridUPoint grid_position = hold_background_position + GridUPoint{ preview_extends.x + 1, 0 };
    constexpr GridUPoint preview_background_position{ grid_position.x + width_in_tiles + 1, 0 };
    constexpr GridUPoint preview_tetromino_position = preview_background_position + GridUPoint{ 0, 1 };
    constexpr GridUPoint hold_tetromino_position = hold_background_position + GridUPoint{ 0, 1 };
    constexpr GridUPoint hold_background_extends = GridUPoint{ 4, 4 };
    constexpr GridUType preview_padding = 3;


} // namespace grid
