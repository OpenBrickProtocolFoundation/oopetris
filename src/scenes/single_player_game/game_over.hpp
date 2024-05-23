#pragma once

#include "scenes/scene.hpp"
#include "ui/components/label.hpp"

namespace scenes {

    struct SinglePlayerGameOver : public Scene {
    private:
        bool m_should_exit{ false };
        ui::Label m_text;
        std::shared_ptr<input::GameInput> m_game_input;

    public:
        explicit SinglePlayerGameOver(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::shared_ptr<input::GameInput>& game_input
        );

        [[nodiscard]] UpdateResult update() override;

        void render(const ServiceProvider& service_provider) override;

        bool handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };

} // namespace scenes
