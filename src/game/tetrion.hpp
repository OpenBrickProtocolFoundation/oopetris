#pragma once

#include <core/game/mino_stack.hpp>
#include <core/helper/random.hpp>
#include <core/helper/types.hpp>
#include <recordings/utility/tetrion_core_information.hpp>

#include "helper/export_symbols.hpp"
#include "input/game_input.hpp"
#include "manager/service_provider.hpp"
#include "simulated_tetrion.hpp"
#include "ui/layout.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

namespace recorder {
    struct RecordingWriter;
}


struct Tetrion final : public ui::Widget, SimulatedTetrion {
private:
    using ScreenCordsFunction = Mino::ScreenCordsFunction;

    ui::TileLayout m_main_layout;

public:
    OOPETRIS_GRAPHICS_EXPORTED Tetrion(
            u8 tetrion_index,
            Random::Seed random_seed,
            u32 starting_level,
            ServiceProvider* service_provider,
            std::optional<std::shared_ptr<recorder::RecordingWriter>> recording_writer,
            const ui::Layout& layout,
            bool is_top_level
    );

    OOPETRIS_GRAPHICS_EXPORTED ~Tetrion() override;

    OOPETRIS_GRAPHICS_EXPORTED Tetrion(const Tetrion& other) = delete;
    OOPETRIS_GRAPHICS_EXPORTED Tetrion& operator=(const Tetrion& other) = delete;

    OOPETRIS_GRAPHICS_EXPORTED Tetrion(Tetrion&& other) noexcept = delete;
    OOPETRIS_GRAPHICS_EXPORTED Tetrion& operator=(Tetrion&& other) noexcept = delete;

    OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED Widget::EventHandleResult
    handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED Grid* get_grid();
    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const Grid* get_grid() const;
    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED ui::GridLayout* get_text_layout();
    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const ui::GridLayout* get_text_layout() const;

private:
    void refresh_texts() override;
};
