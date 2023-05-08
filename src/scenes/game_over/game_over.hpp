#pragma once

#include "../scene.hpp"

namespace scenes {

    struct GameOver : public Scene {
    private:
        bool m_should_exit{ false };

    public:
        explicit GameOver(ServiceProvider* serviceProvider);
        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event) override;
    };

} // namespace scenes
