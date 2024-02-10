#pragma once

#include "../../event_listener.hpp"
#include "../scene.hpp"
#include "label.hpp"

namespace scenes {
    struct Pause : public scenes::Scene {
    private:
        ui::Label m_heading;
        bool m_should_unpause{ false };
        bool m_should_exit{ false };

    public:
        explicit Pause(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        [[nodiscard]] bool handle_event(const SDL_Event& event, const Window* window) override;
    };
} // namespace scenes
