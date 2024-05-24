#include "loading_screen.hpp"
#include "game/graphic_helpers.hpp"
#include "game/tetromino_type.hpp"
#include "graphics/point.hpp"
#include "graphics/renderer.hpp"
#include "graphics/window.hpp"
#include "helper/platform.hpp"

#include <numbers>

scenes::LoadingScreen::LoadingScreen(Window* window)
    : m_segments{
          { Mino{ Mino::GridPoint{ 0, 0 }, helper::TetrominoType::J }, 1.0 },
          { Mino{ Mino::GridPoint{ 1, 0 }, helper::TetrominoType::L }, 1.0 },
          { Mino{ Mino::GridPoint{ 2, 0 }, helper::TetrominoType::I }, 1.0 },
          { Mino{ Mino::GridPoint{ 2, 1 }, helper::TetrominoType::O }, 1.0 },
          { Mino{ Mino::GridPoint{ 2, 2 }, helper::TetrominoType::S }, 1.0 },
          { Mino{ Mino::GridPoint{ 1, 2 }, helper::TetrominoType::T }, 1.0 },
          { Mino{ Mino::GridPoint{ 0, 2 }, helper::TetrominoType::I }, 1.0 },
          { Mino{ Mino::GridPoint{ 0, 1 }, helper::TetrominoType::Z }, 1.0 },
} {

    const auto [total_x_tiles, total_y_tiles] = utils::get_orientation() == utils::Orientation::Landscape
                                                        ? std::pair<u32, u32>{ 17, 9 }
                                                        : std::pair<u32, u32>{ 9, 17 };

    constexpr auto loading_segments_size = 3;

    const auto layout = window->size();

    const u32 tile_size_x = layout.x / total_x_tiles;
    const u32 tile_size_y = layout.y / total_y_tiles;

    m_tile_size = std::min(tile_size_y, tile_size_x);

    const shapes::UPoint grid_start_offset = { (total_x_tiles - loading_segments_size) / 2,
                                               (total_y_tiles - loading_segments_size) / 2 };

    m_start_offset = grid_start_offset * m_tile_size;
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

        scale = std::min(amplitude * std::sin(time * speed + offset) + scale_offset, 1.0);
    }
    //
}

void scenes::LoadingScreen::render(const ServiceProvider& service_provider) const {

    service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black());

    constexpr const auto scale_threshold = 0.25;

    for (const auto& [mino, scale] : m_segments) {
        if (scale >= scale_threshold) {
            const auto original_scale =
                    static_cast<double>(m_tile_size) / static_cast<double>(grid::original_tile_size);


            const auto tile_size = static_cast<u32>(static_cast<double>(m_tile_size) * scale);

            helper::graphics::render_mino(
                    mino, service_provider, MinoTransparency::Solid, original_scale,
                    [this, tile_size](const Mino::GridPoint& point) -> auto {
                        return this->to_screen_coords(point, tile_size);
                    },
                    { tile_size, tile_size }
            );
        }

        //TODO: render text here, but than we need to load the fonts before this, not in the loading thread (not that they take that long)
    }
}


[[nodiscard]] shapes::UPoint scenes::LoadingScreen::to_screen_coords(const Mino::GridPoint& point, u32 tile_size)
        const {
    const auto start_edge = m_start_offset + point.cast<u32>() * m_tile_size;
    const auto inner_offset = m_tile_size - tile_size / 2;
    return start_edge + shapes::UPoint{ inner_offset, inner_offset };
}
