#pragma once

#include <core/core.hpp>
#include <core/helper/color.hpp>

#include "graphics/rect.hpp"
#include "helper/export_symbols.hpp"
#include "manager/service_provider.hpp"
#include "ui/layout.hpp"
#include "ui/widget.hpp"

struct Grid final : public ui::Widget {
public:
    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };
    static constexpr Color grid_color{ 31, 31, 31 };

private:
    using GridRect = shapes::AbstractRect<grid::GridType>;

    shapes::URect m_fill_rect;
    u32 m_tile_size;

public:
    OOPETRIS_GRAPHICS_EXPORTED Grid(const ui::Layout& layout, bool is_top_level);

    [[nodiscard]] shapes::UPoint tile_size() const;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED double scale_to_original() const;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED shapes::UPoint to_screen_coords(grid::GridPoint grid_coords) const;

    OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::BoolWrapper<std::pair<ui::EventHandleType, ui::Widget*>>
    handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

private:
    void draw_preview_background(const ServiceProvider& service_provider) const;
    void draw_hold_background(const ServiceProvider& service_provider) const;
    void draw_playing_field_background(const ServiceProvider& service_provider) const;
    void draw_background(const ServiceProvider& service_provider, GridRect grid_rect) const;
};
