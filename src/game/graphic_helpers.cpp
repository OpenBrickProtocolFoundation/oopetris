#include <core/helper/utils.hpp>

#include "graphic_helpers.hpp"
#include "graphics/renderer.hpp"


#include <array>

static constexpr std::array<u8, 6> transparency_values = { 255, 173, 118, 80, 55, 37 };

namespace {
    [[nodiscard]] u8 get_transparency_value(const MinoTransparency transparency) {
        switch (transparency) {
            case MinoTransparency::Preview0:
            case MinoTransparency::Preview1:
            case MinoTransparency::Preview2:
            case MinoTransparency::Preview3:
            case MinoTransparency::Preview4:
            case MinoTransparency::Preview5:
                return transparency_values.at(static_cast<usize>(transparency));
            case MinoTransparency::Ghost:
            case MinoTransparency::Solid:
                return utils::to_underlying(transparency);
            default:
                UNREACHABLE();
        }
    }
} // namespace


void helper::graphics::render_mino(
        const Mino& mino,
        const ServiceProvider& service_provider,
        const MinoTransparency transparency,
        const double original_scale,
        const Mino::ScreenCordsFunction& to_screen_coords,
        const shapes::UPoint& tile_size,
        const Mino::GridPoint& offset
) {
    const auto alpha = get_transparency_value(transparency);
    const auto alpha_factor = static_cast<double>(alpha) / 255.0;
    const Color foreground = get_foreground_color(mino.type(), alpha);
    const Color background = get_background_color(mino.type(), alpha);

    const auto one_scaled_unit = static_cast<u32>(original_scale);

    const auto inset_scaled = static_cast<int>(original_scale * mino_original_inset);

    const shapes::UPoint top_left = to_screen_coords(mino.position() + offset);
    const shapes::UPoint top_right = top_left + shapes::UPoint{ tile_size.x - one_scaled_unit, 0 };
    const shapes::UPoint bottom_left = top_left + shapes::UPoint{ 0, tile_size.y - one_scaled_unit };
    const shapes::UPoint bottom_right = top_left + tile_size - (shapes::UPoint{ one_scaled_unit, one_scaled_unit });

    service_provider.renderer().draw_rect_filled(shapes::URect{ top_left, bottom_right }, background);

    const shapes::UPoint inner_top_left =
            (top_left.cast<i32>() + shapes::IPoint(inset_scaled, inset_scaled)).cast<u32>();
    const shapes::UPoint inner_top_right =
            (top_right.cast<i32>() + shapes::IPoint(-inset_scaled, inset_scaled)).cast<u32>();
    const shapes::UPoint inner_bottom_left =
            (bottom_left.cast<i32>() + shapes::IPoint(inset_scaled, -inset_scaled)).cast<u32>();
    const shapes::UPoint inner_bottom_right =
            (bottom_right.cast<i32>() - shapes::IPoint(inset_scaled, inset_scaled)).cast<u32>();


    service_provider.renderer().draw_line(
            top_left, inner_top_left, Color::white(static_cast<u8>(140.0 * alpha_factor))
    );
    service_provider.renderer().draw_line(
            bottom_left, inner_bottom_left, Color::white(static_cast<u8>(100.0 * alpha_factor))
    );
    service_provider.renderer().draw_line(top_right, inner_top_right, Color{ 80, 80, 80, alpha });
    service_provider.renderer().draw_line(
            bottom_right, inner_bottom_right, Color{ 80, 80, 80, static_cast<u8>(180.0 * alpha_factor) }
    );

    service_provider.renderer().draw_rect_filled(shapes::URect{ inner_top_left, inner_bottom_right }, foreground);
}


void helper::graphics::render_minos(
        const MinoStack& mino_stack,
        const ServiceProvider& service_provider,
        const double original_scale,
        const Mino::ScreenCordsFunction& to_screen_coords,
        const shapes::UPoint& tile_size
) {
    for (const auto& mino : mino_stack.minos()) {
        render_mino(
                mino, service_provider, MinoTransparency::Solid, original_scale, to_screen_coords, tile_size,
                grid::grid_position
        );
    }
}
