#pragma once

#include <core/game/mino.hpp>

#include "../logo/logo.hpp"
#include "graphics/rect.hpp"
#include "manager/service_provider.hpp"

#include <vector>

namespace scenes {

    struct LoadingScreen {
    private:
        std::vector<std::tuple<Mino, double>> m_segments;
        Texture m_logo;
        shapes::URect m_logo_rect;

        u32 m_tile_size;
        shapes::UPoint m_start_offset;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit LoadingScreen(ServiceProvider* service_provider);

        OOPETRIS_GRAPHICS_EXPORTED void update();

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const;

    private:
        [[nodiscard]] shapes::UPoint to_screen_coords(const grid::GridPoint& point, u32 tile_size) const;
    };

} // namespace scenes
