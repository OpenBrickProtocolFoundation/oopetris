#include "mino.hpp"
#include "grid.hpp"
#include "renderer.hpp"
#include "utils.hpp"
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

void Mino::render(const ServiceProvider& service_provider, const Grid& grid, const MinoTransparency transparency)
        const {
    const auto alpha = get_transparency_value(transparency);
    const auto alpha_factor = static_cast<double>(alpha) / 255.0;
    const Color foreground = get_foreground_color(m_type, alpha);
    const Color background = get_background_color(m_type, alpha);

    const Point top_left = grid.to_screen_coords(m_position);
    const Point top_right = top_left + Point{ static_cast<SDL_int>(grid.tile_size().x - 1), 0 };
    const Point bottom_left = top_left + Point{ 0, static_cast<SDL_int>(grid.tile_size().y - 1) };
    const Point bottom_right = top_left + grid.tile_size() - Point{ 1, 1 };

    service_provider.renderer().draw_rect_filled(Rect{ top_left, bottom_right }, background);

    const Point inner_top_left = top_left + Point{ inset, inset };
    const Point inner_top_right = top_right + Point{ -inset, inset };
    const Point inner_bottom_left = bottom_left + Point{ inset, -inset };
    const Point inner_bottom_right = bottom_right - Point{ inset, inset };


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

    service_provider.renderer().draw_rect_filled(Rect{ inner_top_left, inner_bottom_right }, foreground);
}
