#pragma once

#include "helper/color.hpp"
#include "scenes/scene.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <variant>

namespace details::settings::menu {

    struct Return { };

    struct Action {
        ui::Widget* widget;
    };

    struct Command {
        using Value = std::variant<Return, Action>;
        Value m_value;

        template<typename T>
        explicit Command(T value) : m_value{ std::move(value) } {
            //
        }
    };


} // namespace details::settings::menu

namespace scenes {

    struct SettingsMenu : public Scene {
    private:
        std::optional<details::settings::menu::Command> m_next_command{ std::nullopt };
        ui::TileLayout m_main_layout;
        //todo migrate to settings state
        std::vector<Color> m_colors;
        std::optional<std::shared_ptr<input::GameInput>> m_game_input;

        const std::string listener_name = "settings_menu";

        explicit SettingsMenu(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::optional<std::shared_ptr<input::GameInput>>& game_input
        );

    public:
        explicit SettingsMenu(ServiceProvider* service_provider, const ui::Layout& layout);
        explicit SettingsMenu(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::shared_ptr<input::GameInput>& game_input
        );

        [[nodiscard]] UpdateResult update() override;

        void render(const ServiceProvider& service_provider) override;

        bool handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        void on_unhover() override;
    };

} // namespace scenes
