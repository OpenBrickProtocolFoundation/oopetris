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
    using GridType = u8;
    using GridPoint = shapes::AbstractPoint<GridType>;
    using GridRect = shapes::AbstractRect<GridType>;

    static constexpr GridType original_tile_size = 26;

    static constexpr GridType width_in_tiles = 10;
    static constexpr GridType height_in_tiles = 20;

    static constexpr GridPoint preview_extends{ 4, (6 * 3) + 1 };
    static constexpr GridPoint hold_background_position{ 0, 0 };
    static constexpr GridPoint grid_position = hold_background_position + GridPoint{ preview_extends.x + 1, 0 };
    static constexpr GridPoint preview_background_position{ grid_position.x + width_in_tiles + 1, 0 };
    static constexpr GridPoint preview_tetromino_position = preview_background_position + GridPoint{ 0, 1 };
    static constexpr GridPoint hold_tetromino_position = hold_background_position + GridPoint{ 0, 1 };
    static constexpr GridPoint hold_background_extends = GridPoint{ 4, 4 };
    static constexpr GridType preview_padding = 3;

    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };
    static constexpr Color grid_color{ 31, 31, 31 };

private:
    shapes::URect m_fill_rect;
    u32 m_tile_size;

public:
    Grid(const ui::Layout& layout);
    [[nodiscard]] shapes::UPoint tile_size() const;
    [[nodiscard]] double scale_to_original() const;
    [[nodiscard]] shapes::UPoint to_screen_coords(GridPoint grid_coords) const;
    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] helper::BoolWrapper<ui::EventHandleType> handle_event(const SDL_Event& event, const Window* window)
            override;

private:
    void draw_preview_background(const ServiceProvider& service_provider) const;
    void draw_hold_background(const ServiceProvider& service_provider) const;
    void draw_playing_field_background(const ServiceProvider& service_provider) const;
    void draw_background(const ServiceProvider& service_provider, GridRect grid_rect) const;
};
