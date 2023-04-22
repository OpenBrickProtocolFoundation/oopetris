#pragma once

#include "application.hpp"
#include "mino.hpp"
#include "point.hpp"
#include <algorithm>
#include <vector>

struct Grid final {
public:
    static constexpr int width = 10;
    static constexpr int height = 22;
    static constexpr int invisible_rows = 2;
    static constexpr Point preview_background_position{ width + 1, 1 + invisible_rows };
    static constexpr Point preview_tetromino_position = preview_background_position + Point{ 0, 1 };
    static constexpr Point preview_extends{ 4, 4 };
    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };
    static constexpr Color grid_color{ 31, 31, 31 };
    static constexpr Point hold_background_position{ width + 1, 1 + invisible_rows + height - preview_extends.y - 4 };
    static constexpr Point hold_tetromino_position = hold_background_position + Point{ 0, 1 };

private:
    Point m_offset;
    int m_tile_size;

public:
    Grid(Point offset, int tile_size);
    [[nodiscard]] Point tile_size() const;
    [[nodiscard]] Point to_screen_coords(Point grid_coords) const;
    void render(const Application& app) const;

private:
    void draw_preview_background(const Application& app) const;
    void draw_hold_background(const Application& app) const;
    void draw_small_background(const Application& app, Point position) const;
    void draw_playing_field_background(const Application& app) const;
};
