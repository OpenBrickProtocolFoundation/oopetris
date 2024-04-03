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

        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
