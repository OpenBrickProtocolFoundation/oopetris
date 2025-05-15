#pragma once

#include <core/core.hpp>

#include "helper/export_symbols.hpp"
#include "manager/service_provider.hpp"

enum class MinoTransparency : u8 {
    // here the enum value is used as index into the preview alpha array
    Preview0,
    Preview1,
    Preview2,
    Preview3,
    Preview4,
    Preview5,
    // here the enum value is used as alpha!
    Ghost = 50,
    Solid = 255,
};

namespace helper::graphics {
    static constexpr int mino_original_inset = 3;

    OOPETRIS_GRAPHICS_EXPORTED void render_mino(
            const Mino& mino,
            const ServiceProvider& service_provider,
            MinoTransparency transparency,
            double original_scale,
            const Mino::ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size,
            const grid::GridPoint& offset = grid::GridPoint::zero()
    );


    OOPETRIS_GRAPHICS_EXPORTED void render_minos(
            const MinoStack& mino_stack,
            const ServiceProvider& service_provider,
            double original_scale,
            const Mino::ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size
    );
}; // namespace helper::graphics
