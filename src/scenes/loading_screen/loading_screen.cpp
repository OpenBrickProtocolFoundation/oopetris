#include <core/game/tetromino_type.hpp>
#include <core/helper/point.hpp>

#include "game/graphic_helpers.hpp"
#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "graphics/window.hpp"
#include "helper/platform.hpp"
#include "loading_screen.hpp"
#include "manager/service_provider.hpp"
#include "scenes/logo/logo.hpp"
#include "ui/layout.hpp"

#include <numbers>

scenes::LoadingScreen::LoadingScreen(ServiceProvider* service_provider)
    : m_segments{
          { Mino{ grid::GridUPoint{ 0, 0 }, helper::TetrominoType::J }, 1.0 },
          { Mino{ grid::GridUPoint{ 1, 0 }, helper::TetrominoType::L }, 1.0 },
          { Mino{ grid::GridUPoint{ 2, 0 }, helper::TetrominoType::I }, 1.0 },
          { Mino{ grid::GridUPoint{ 2, 1 }, helper::TetrominoType::O }, 1.0 },
          { Mino{ grid::GridUPoint{ 2, 2 }, helper::TetrominoType::S }, 1.0 },
          { Mino{ grid::GridUPoint{ 1, 2 }, helper::TetrominoType::T }, 1.0 },
          { Mino{ grid::GridUPoint{ 0, 2 }, helper::TetrominoType::I }, 1.0 },
          { Mino{ grid::GridUPoint{ 0, 1 }, helper::TetrominoType::Z }, 1.0 },
},m_logo{logo::get_logo(service_provider)} {

    const auto [total_x_tiles, total_y_tiles] = utils::get_orientation() == utils::Orientation::Landscape
                                                        ? std::pair<u32, u32>{ 17, 9 }
                                                        : std::pair<u32, u32>{ 9, 17 };

    constexpr auto loading_segments_size = 3;

    const auto& window = service_provider->window();

    const auto layout = window.size();

    const u32 tile_size_x = layout.x / total_x_tiles;
    const u32 tile_size_y = layout.y / total_y_tiles;

    m_tile_size = std::min(tile_size_y, tile_size_x);

    const shapes::UPoint grid_start_offset = { (total_x_tiles - loading_segments_size) / 2,
                                               (total_y_tiles - loading_segments_size) / 2 };

    m_start_offset = grid_start_offset * m_tile_size;

    constexpr const auto logo_width_percentage = 0.8;

    constexpr const auto start_x = (1.0 - logo_width_percentage) / 2.0;

    const auto window_ratio = static_cast<double>(layout.x) / static_cast<double>(layout.y);

    const auto logo_ratio = static_cast<double>(logo::height) / static_cast<double>(logo::width) * window_ratio;

    const auto logo_height_percentage = logo_width_percentage * logo_ratio;

    m_logo_rect = ui::RelativeLayout(window, start_x, 0.05, logo_width_percentage, logo_height_percentage).get_rect();
}

namespace {
    [[nodiscard]] double elapsed_time() {
        return static_cast<double>(SDL_GetTicks64()) / 1000.0;
    }
} // namespace


void scenes::LoadingScreen::update() {

    constexpr const auto speed = std::numbers::pi_v<double> * 1.0;
    constexpr const auto amplitude = 1.1;
    constexpr const auto scale_offset = 1.3;

    const auto length = m_segments.size();
    const auto length_d = static_cast<double>(length);

    const auto time = elapsed_time();

    for (size_t i = 0; i < length; ++i) {

        auto& segment = m_segments.at(i);

        auto& scale = std::get<1>(segment);

        const auto offset = std::numbers::pi_v<double> * 2.0 * static_cast<double>(length - i - 1) / length_d;

        scale = std::min((amplitude * std::sin((time * speed) + offset)) + scale_offset, 1.0);
    }
    //
}

void scenes::LoadingScreen::render(const ServiceProvider& service_provider) const {

    service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black());

    service_provider.renderer().draw_texture(m_logo, m_logo_rect);

    constexpr const auto scale_threshold = 0.25;

    for (const auto& [mino, scale] : m_segments) {
        if (scale >= scale_threshold) {
            const auto original_scale =
                    static_cast<double>(m_tile_size) / static_cast<double>(grid::original_tile_size);


            const auto tile_size = static_cast<u32>(static_cast<double>(m_tile_size) * scale);

            helper::graphics::render_mino(
                    mino, service_provider, MinoTransparency::Solid, original_scale,
                    [this, tile_size](const grid::GridUPoint& point) -> auto {
                        return this->to_screen_coords(point, tile_size);
                    },
                    { tile_size, tile_size }
            );
        }

        //TODO(Totto): render text here, but than we need to load the fonts before this, not in the loading thread (not that they take that long)
    }
}


[[nodiscard]] shapes::UPoint scenes::LoadingScreen::to_screen_coords(const grid::GridUPoint& point, u32 tile_size)
        const {
    const auto start_edge = m_start_offset + point.cast<u32>() * m_tile_size;
    const auto inner_offset = m_tile_size - (tile_size / 2);
    return start_edge + shapes::UPoint{ inner_offset, inner_offset };
}
