

#pragma once

#include "../helper/point.hpp"

namespace grid {

    using GridType = u8;
    using GridPoint = shapes::AbstractPoint<GridType>;

    constexpr GridType original_tile_size = 26;

    constexpr GridType width_in_tiles = 10;
    constexpr GridType height_in_tiles = 20;

    constexpr GridPoint preview_extends{ 4, (6 * 3) + 1 };
    constexpr GridPoint hold_background_position{ 0, 0 };
    constexpr GridPoint grid_position = hold_background_position + GridPoint{ preview_extends.x + 1, 0 };
    constexpr GridPoint preview_background_position{ grid_position.x + width_in_tiles + 1, 0 };
    constexpr GridPoint preview_tetromino_position = preview_background_position + GridPoint{ 0, 1 };
    constexpr GridPoint hold_tetromino_position = hold_background_position + GridPoint{ 0, 1 };
    constexpr GridPoint hold_background_extends = GridPoint{ 4, 4 };
    constexpr GridType preview_padding = 3;


} // namespace grid
