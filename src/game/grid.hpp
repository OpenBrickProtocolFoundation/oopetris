#pragma once

#include "graphics/point.hpp"
#include "graphics/rect.hpp"
#include "grid_properties.hpp"
#include "helper/color.hpp"
#include "manager/service_provider.hpp"
#include "ui/layout.hpp"
#include "ui/widget.hpp"

struct Grid final : public ui::Widget {
public:
    using GridType = grid::GridType;
    using GridPoint = grid::GridPoint;
    using GridRect = shapes::AbstractRect<GridType>;

    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };
    static constexpr Color grid_color{ 31, 31, 31 };

private:
    shapes::URect m_fill_rect;
    u32 m_tile_size;

public:
    Grid(const ui::Layout& layout, bool is_top_level);
    [[nodiscard]] shapes::UPoint tile_size() const;
    [[nodiscard]] double scale_to_original() const;
    [[nodiscard]] shapes::UPoint to_screen_coords(GridPoint grid_coords) const;
    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] helper::BoolWrapper<std::pair<ui::EventHandleType, ui::Widget*>>
    handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

private:
    void draw_preview_background(const ServiceProvider& service_provider) const;
    void draw_hold_background(const ServiceProvider& service_provider) const;
    void draw_playing_field_background(const ServiceProvider& service_provider) const;
    void draw_background(const ServiceProvider& service_provider, GridRect grid_rect) const;
};
