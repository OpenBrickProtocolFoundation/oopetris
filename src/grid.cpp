#include "grid.hpp"

constexpr int Grid::width = 10;
constexpr int Grid::height = 22;
constexpr int Grid::invisible_rows = 2;
constexpr int Grid::num_tiles = width * height;
constexpr Point Grid::preview_background_position{ width + 1, 1 + invisible_rows };
constexpr Point Grid::preview_tetromino_position = preview_background_position + Point{ 0, 1 };
constexpr Point Grid::preview_extends{ 4, 4 };
constexpr Color Grid::background_color{ 12, 12, 12 };
constexpr Color Grid::border_color{ 42, 42, 42 };
