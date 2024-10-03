#pragma once

#include "manager/event_listener.hpp"
#include "scenes/scene.hpp"
#include "ui/components/label.hpp"

namespace scenes {
    struct SinglePlayerPause : public scenes::Scene {
    private:
        ui::Label m_heading;
        bool m_should_unpause{ false };
        bool m_should_exit{ false };
        std::shared_ptr<input::GameInput> m_game_input;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit SinglePlayerPause(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::shared_ptr<input::GameInput>& game_input
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] UpdateResult update() override;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };
} // namespace scenes
