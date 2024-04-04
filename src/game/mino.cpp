#include "mino.hpp"
#include "helper/utils.hpp"

#if defined(_HAVE_SDL)
#include "graphics/renderer.hpp"
#endif

#include <array>

#if defined(_HAVE_SDL)
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
        const MinoTransparency transparency,
        const double original_scale,
        const ScreenCordsFunction& to_screen_coords,
        const shapes::UPoint& tile_size,
        const GridPoint& offset
) const {
    const auto alpha = get_transparency_value(transparency);
    const auto alpha_factor = static_cast<double>(alpha) / 255.0;
    const Color foreground = get_foreground_color(m_type, alpha);
    const Color background = get_background_color(m_type, alpha);

    const auto one_scaled_unit = static_cast<u32>(original_scale);

    const auto inset_scaled = static_cast<int>(original_scale * original_inset);

    const shapes::UPoint top_left = to_screen_coords(m_position + offset);
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
#endif

[[nodiscard]] helper::TetrominoType Mino::type() const {
    return m_type;
}

[[nodiscard]] const Mino::GridPoint& Mino::position() const {
    return m_position;
}

[[nodiscard]] Mino::GridPoint& Mino::position() {
    return m_position;
}

[[nodiscard]] bool Mino::operator==(const Mino& other) const {
    return m_position == other.m_position and m_type == other.m_type;
}

[[nodiscard]] bool Mino::operator!=(const Mino& other) const {
    return not(*this == other);
}
