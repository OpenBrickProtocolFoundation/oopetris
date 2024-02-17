#pragma once

#include "graphics/point.hpp"
#include "graphics/rect.hpp"
#include "manager/service_provider.hpp"
#include "mino.hpp"
#include "ui/layout.hpp"
#include "ui/widget.hpp"

#include <algorithm>
#include <vector>

struct Grid final : public ui::Widget {
public:
    static constexpr usize width_in_tiles = 10;
    static constexpr usize height_in_tiles = 22;
    static constexpr usize invisible_rows = 2;

    static constexpr shapes::Point preview_extends{ 4, 6 * 3 + 1 };
    static constexpr shapes::Point hold_background_position{ 0, 0 };
    static constexpr shapes::Point grid_position = hold_background_position + shapes::Point{ preview_extends.x + 1, 0 };
    static constexpr shapes::Point preview_background_position{ grid_position.x + width_in_tiles + 1, 0 };
    static constexpr shapes::Point preview_tetromino_position = preview_background_position + shapes::Point{ 0, 3 };
    static constexpr shapes::Point hold_tetromino_position = hold_background_position + shapes::Point{ 0, 3 };
    static constexpr shapes::Point hold_background_extends = shapes::Point{ 4, 4 };
    static constexpr u32 preview_padding = 3;

    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };
    static constexpr Color grid_color{ 31, 31, 31 };

private:
    shapes::Rect m_fill_rect;
    u32 m_tile_size;

public:
    Grid(const ui::Layout& layout);
    [[nodiscard]] shapes::Point tile_size() const;
    [[nodiscard]] shapes::Point to_screen_coords(shapes::Point grid_coords) const;
    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] bool handle_event(const SDL_Event& event, const Window* window) override;

private:
    void draw_preview_background(const ServiceProvider& service_provider) const;
    void draw_hold_background(const ServiceProvider& service_provider) const;
    void draw_playing_field_background(const ServiceProvider& service_provider) const;
    void draw_background(const ServiceProvider& service_provider, shapes::Rect grid_rect) const;
};
