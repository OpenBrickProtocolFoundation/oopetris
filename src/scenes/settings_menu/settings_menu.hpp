#pragma once

#include "helper/color.hpp"
#include "scenes/scene.hpp"
#include "ui/layouts/tile_layout.hpp"

namespace scenes {

    struct SettingsMenu : public Scene {
    private:
        enum class Command : u8 { Action, Return };

        helper::optional<Command> m_next_command{ helper::nullopt };
        ui::TileLayout m_main_layout;
        //todo migrate to settings state
        std::vector<Color> m_colors;

        const std::string listener_name = "settings_menu";

    public:
        explicit SettingsMenu(ServiceProvider* service_provider, const ui::Layout& layout);

        [[nodiscard]] UpdateResult update() override;

        void render(const ServiceProvider& service_provider) override;

        bool handle_event(const SDL_Event& event, const Window* window) override;

        void on_unhover() override;
    };

} // namespace scenes
