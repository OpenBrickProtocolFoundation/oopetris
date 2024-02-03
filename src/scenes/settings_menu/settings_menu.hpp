#pragma once

#include "../../ui/button.hpp"
#include "../../ui/grid.hpp"
#include "../../ui/label.hpp"
#include "../../ui/slider.hpp"
#include "../scene.hpp"

namespace scenes {

    struct SettingsMenu : public Scene {
    private:
        bool m_should_exit = false;
        ui::Grid<3> m_main_grid;

    public:
        explicit SettingsMenu(ServiceProvider* service_provider);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
