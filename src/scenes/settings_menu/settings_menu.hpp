#pragma once

#include "../scene.hpp"
#include "button.hpp"
#include "grid_layout.hpp"
#include "label.hpp"
#include "slider.hpp"

namespace scenes {

    struct SettingsMenu : public Scene {
    private:
        bool m_should_exit = false;
        ui::GridLayout<4> m_main_grid;

        const std::string listener_name = "settings_menu";

    public:
        explicit SettingsMenu(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
