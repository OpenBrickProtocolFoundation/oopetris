#pragma once

#include "../../ui/button.hpp"
#include "../../ui/focus_group.hpp"
#include "../../ui/label.hpp"
#include "../scene.hpp"

namespace scenes {

    struct SettingsMenu : public Scene {
    private:
        bool m_should_exit = false;
        ui::Label m_heading;
        ui::FocusGroup m_focus_group;

    public:
        explicit SettingsMenu(ServiceProvider* service_provider, Window* window);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event) override;
    };

} // namespace scenes
