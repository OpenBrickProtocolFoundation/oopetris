#pragma once

#include <core/game/mino.hpp>

#include "graphics/rect.hpp"
#include "manager/service_provider.hpp"
#include "ui/widget.hpp"

#include <vector>

namespace ui {

    struct IndeterminateSpinner final : public Widget {
    private:
        std::vector<std::tuple<Mino, double>> m_segments;

        u32 m_tile_size;
        shapes::UPoint m_start_offset;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit IndeterminateSpinner(const Layout& layout, bool is_top_level);

        OOPETRIS_GRAPHICS_EXPORTED void update() override;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

    private:
        [[nodiscard]] shapes::UPoint to_screen_coords(const grid::GridPoint& point, u32 tile_size) const;
    };

} // namespace ui
