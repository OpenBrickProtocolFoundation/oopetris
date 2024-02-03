#pragma once

#include "../../ui/button.hpp"
#include "../../ui/grid.hpp"
#include "../../ui/label.hpp"
#include "../scene.hpp"

namespace scenes {

    struct MainMenu : public Scene {
    private:
        enum class Command {
            StartGame,
            OpenSettingsMenu,
            Exit,
        };

        ui::Grid<4> m_main_grid;
        tl::optional<Command> m_next_command;

    public:
        explicit MainMenu(ServiceProvider* service_provider);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event, const Window* window) override;
    };

} // namespace scenes
