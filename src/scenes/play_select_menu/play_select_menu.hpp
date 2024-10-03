#pragma once

#include "scenes/scene.hpp"
#include "ui/components/label.hpp"
#include "ui/components/text_button.hpp"
#include "ui/layouts/grid_layout.hpp"

namespace scenes {

    struct PlaySelectMenu : public Scene {
    private:
        enum class Command : u8 { SinglePlayer, MultiPlayer, RecordingSelector, Return };

        ui::GridLayout m_main_grid;
        std::optional<Command> m_next_command;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit PlaySelectMenu(ServiceProvider* service_provider, const ui::Layout& layout);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] UpdateResult update() override;
        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) override;
        OOPETRIS_GRAPHICS_EXPORTED bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };

} // namespace scenes
