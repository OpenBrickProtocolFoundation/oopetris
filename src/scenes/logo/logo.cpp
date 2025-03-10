#include <core/game/mino.hpp>
#include <core/helper/point.hpp>

#include "game/graphic_helpers.hpp"
#include "graphics/renderer.hpp"
#include "logo.hpp"

#include <vector>


[[nodiscard]] Texture logo::get_logo(const ServiceProvider* service_provider, double scale) {

    constexpr const grid::GridPoint offset_o2{ 5, 0 };
    constexpr const auto offset_p3 = offset_o2 + grid::GridPoint{ 5, 0 };
    constexpr const auto offset_e4 = offset_p3 + grid::GridPoint{ 4, 0 };
    constexpr const auto offset_t5 = offset_e4 + grid::GridPoint{ 4, 0 };
    constexpr const auto offset_r6 = offset_t5 + grid::GridPoint{ 6, 0 };
    constexpr const auto offset_i7 = offset_r6 + grid::GridPoint{ 4, 0 };
    constexpr const auto offset_s8 = offset_i7 + grid::GridPoint{ 2, 0 };

    const std::vector<Mino> minos{
        // O
        Mino{             grid::GridPoint{ 0, 0 }, helper::TetrominoType::J },
        Mino{             grid::GridPoint{ 1, 0 }, helper::TetrominoType::J },
        Mino{             grid::GridPoint{ 2, 0 }, helper::TetrominoType::L },
        Mino{             grid::GridPoint{ 3, 0 }, helper::TetrominoType::L },
        Mino{             grid::GridPoint{ 3, 1 }, helper::TetrominoType::L },
        Mino{             grid::GridPoint{ 3, 2 }, helper::TetrominoType::L },
        Mino{             grid::GridPoint{ 3, 3 }, helper::TetrominoType::Z },
        Mino{             grid::GridPoint{ 3, 4 }, helper::TetrominoType::I },
        Mino{             grid::GridPoint{ 2, 4 }, helper::TetrominoType::I },
        Mino{             grid::GridPoint{ 1, 4 }, helper::TetrominoType::I },
        Mino{             grid::GridPoint{ 0, 4 }, helper::TetrominoType::I },
        Mino{             grid::GridPoint{ 0, 3 }, helper::TetrominoType::S },
        Mino{             grid::GridPoint{ 0, 2 }, helper::TetrominoType::J },
        Mino{             grid::GridPoint{ 0, 1 }, helper::TetrominoType::J },
        // O
        Mino{ grid::GridPoint{ 0, 0 } + offset_o2, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 1, 0 } + offset_o2, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 2, 0 } + offset_o2, helper::TetrominoType::S },
        Mino{ grid::GridPoint{ 3, 0 } + offset_o2, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 3, 1 } + offset_o2, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 3, 2 } + offset_o2, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 3, 3 } + offset_o2, helper::TetrominoType::S },
        Mino{ grid::GridPoint{ 3, 4 } + offset_o2, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 2, 4 } + offset_o2, helper::TetrominoType::S },
        Mino{ grid::GridPoint{ 1, 4 } + offset_o2, helper::TetrominoType::S },
        Mino{ grid::GridPoint{ 0, 4 } + offset_o2, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 0, 3 } + offset_o2, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 0, 2 } + offset_o2, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 0, 1 } + offset_o2, helper::TetrominoType::O },
        // P
        Mino{ grid::GridPoint{ 0, 0 } + offset_p3, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 1, 0 } + offset_p3, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 2, 0 } + offset_p3, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 2, 1 } + offset_p3, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 2, 2 } + offset_p3, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 1, 2 } + offset_p3, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 2 } + offset_p3, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 1 } + offset_p3, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 3 } + offset_p3, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 4 } + offset_p3, helper::TetrominoType::S },
        // E
        Mino{ grid::GridPoint{ 0, 0 } + offset_e4, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 1, 0 } + offset_e4, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 2, 0 } + offset_e4, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 0, 1 } + offset_e4, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 0, 2 } + offset_e4, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 1, 2 } + offset_e4, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 2, 2 } + offset_e4, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 0, 3 } + offset_e4, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 4 } + offset_e4, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 1, 4 } + offset_e4, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 2, 4 } + offset_e4, helper::TetrominoType::S },
        // T
        Mino{ grid::GridPoint{ 0, 0 } + offset_t5, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 1, 0 } + offset_t5, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 2, 0 } + offset_t5, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 3, 0 } + offset_t5, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 4, 0 } + offset_t5, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 2, 1 } + offset_t5, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 2, 2 } + offset_t5, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 2, 3 } + offset_t5, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 2, 4 } + offset_t5, helper::TetrominoType::S },
        // R
        Mino{ grid::GridPoint{ 0, 0 } + offset_r6, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 1, 0 } + offset_r6, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 2, 0 } + offset_r6, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 2, 1 } + offset_r6, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 2, 2 } + offset_r6, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 1, 2 } + offset_r6, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 2 } + offset_r6, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 0, 1 } + offset_r6, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 0, 3 } + offset_r6, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 1, 3 } + offset_r6, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 0, 4 } + offset_r6, helper::TetrominoType::S },
        Mino{ grid::GridPoint{ 2, 4 } + offset_r6, helper::TetrominoType::S },
        // I
        Mino{ grid::GridPoint{ 0, 0 } + offset_i7, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 0, 1 } + offset_i7, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 0, 2 } + offset_i7, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 0, 3 } + offset_i7, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 0, 4 } + offset_i7, helper::TetrominoType::S },
        // S
        Mino{ grid::GridPoint{ 0, 0 } + offset_s8, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 1, 0 } + offset_s8, helper::TetrominoType::Z },
        Mino{ grid::GridPoint{ 2, 0 } + offset_s8, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 2, 2 } + offset_s8, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 1, 2 } + offset_s8, helper::TetrominoType::L },
        Mino{ grid::GridPoint{ 0, 2 } + offset_s8, helper::TetrominoType::J },
        Mino{ grid::GridPoint{ 0, 1 } + offset_s8, helper::TetrominoType::I },
        Mino{ grid::GridPoint{ 2, 3 } + offset_s8, helper::TetrominoType::T },
        Mino{ grid::GridPoint{ 0, 4 } + offset_s8, helper::TetrominoType::S },
        Mino{ grid::GridPoint{ 1, 4 } + offset_s8, helper::TetrominoType::O },
        Mino{ grid::GridPoint{ 2, 4 } + offset_s8, helper::TetrominoType::I },
    };

    const auto tile_size = static_cast<u32>(static_cast<double>(grid::original_tile_size) * scale);


    const shapes::UPoint total_size{ tile_size * logo::width, tile_size * logo::height };

    const auto original_scale = static_cast<double>(tile_size) / static_cast<double>(grid::original_tile_size);


    const auto& renderer = service_provider->renderer();

    auto texture = renderer.get_texture_for_render_target(total_size);


    renderer.set_render_target(texture);
    renderer.clear();

    for (const auto& mino : minos) {
        helper::graphics::render_mino(
                mino, *service_provider, MinoTransparency::Solid, original_scale,
                [tile_size](const grid::GridPoint& point) -> auto { return point.cast<u32>() * tile_size; },
                { tile_size, tile_size }
        );
    }


    renderer.reset_render_target();


    return texture;
}
