#pragma once

#include "scenes/scene.hpp"
#include "ui/components/label.hpp"

namespace scenes {

    struct GameOver : public Scene {
    private:
        bool m_should_exit{ false };
        ui::Label m_text;

    public:
        explicit GameOver(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;

        void render(const ServiceProvider& service_provider) override;

        bool handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;
    };

} // namespace scenes
