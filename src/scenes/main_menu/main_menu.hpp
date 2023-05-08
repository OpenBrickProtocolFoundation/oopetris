#pragma once

#include "../../ui/button.hpp"
#include "../../ui/focus_group.hpp"
#include "../../ui/label.hpp"
#include "../scene.hpp"

namespace scenes {

    struct MainMenu : public Scene {
    private:
        enum class Command {
            StartGame,
            Exit,
        };

    private:
        ui::Label m_heading;
        ui::FocusGroup m_focus_group;
        tl::optional<Command> m_next_command;

    public:
        explicit MainMenu(ServiceProvider* service_provider);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        bool handle_event(const SDL_Event& event) override;
    };

} // namespace scenes
