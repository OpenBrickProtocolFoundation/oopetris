#include "mino.hpp"
#include "graphics/renderer.hpp"
#include "grid.hpp"
#include "helper/utils.hpp"
#include <cassert>

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
                utils::unreachable();
        }
    }
} // namespace

void Mino::render(
        const ServiceProvider& service_provider,
        const Grid* grid,
        const MinoTransparency transparency,
        const shapes::Point& offset
) const {
    //TODO: get rid of the offset!

    const auto alpha = get_transparency_value(transparency);
    const auto alpha_factor = static_cast<double>(alpha) / 255.0;
    const Color foreground = get_foreground_color(m_type, alpha);
    const Color background = get_background_color(m_type, alpha);

    const shapes::Point top_left = grid->to_screen_coords(m_position + offset);
    const shapes::Point top_right = top_left + shapes::Point{ grid->tile_size().x - 1, 0 };
    const shapes::Point bottom_left = top_left + shapes::Point{ 0, grid->tile_size().y - 1 };
    const shapes::Point bottom_right = top_left + grid->tile_size() - shapes::Point{ 1, 1 };

    service_provider.renderer().draw_rect_filled(shapes::Rect{ top_left, bottom_right }, background);

    const shapes::Point inner_top_left = top_left + shapes::Point{ inset, inset };
    const shapes::Point inner_top_right = top_right + shapes::Point{ -inset, inset };
    const shapes::Point inner_bottom_left = bottom_left + shapes::Point{ inset, -inset };
    const shapes::Point inner_bottom_right = bottom_right - shapes::Point{ inset, inset };


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

    service_provider.renderer().draw_rect_filled(shapes::Rect{ inner_top_left, inner_bottom_right }, foreground);
}
