#pragma once

#include "mino.hpp"
#include "point.hpp"
#include "rect.hpp"
#include "service_provider.hpp"
#include <algorithm>
#include <vector>

struct Grid final {
public:
    static constexpr usize width = 10;
    static constexpr usize height = 22;
    static constexpr usize invisible_rows = 2;
    static constexpr Point preview_background_position{ width + 1, 0 };
    static constexpr Point preview_tetromino_position = preview_background_position + Point{ 0, 3 };
    static constexpr Point preview_extends{ 4, 6 * 3 + 1 };
    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };
    static constexpr Color grid_color{ 31, 31, 31 };
    static constexpr Point hold_background_position{ -preview_extends.x - 1, 0 };
    static constexpr Point hold_tetromino_position = hold_background_position + Point{ 0, 3 };
    static constexpr Point hold_background_extends = Point{ 4, 4 };
    static constexpr usize preview_padding = 3;

private:
    Point m_offset;
    usize m_tile_size;

public:
    Grid(Point offset, usize tile_size);
    [[nodiscard]] Point tile_size() const;
    [[nodiscard]] Point to_screen_coords(Point grid_coords) const;
    void render(const ServiceProvider& service_provider) const;

private:
    void draw_preview_background(const ServiceProvider& service_provider) const;
    void draw_hold_background(const ServiceProvider& service_provider) const;
    void draw_playing_field_background(const ServiceProvider& service_provider) const;
    void draw_background(const ServiceProvider& service_provider, Rect grid_rect) const;
};
