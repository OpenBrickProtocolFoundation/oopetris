#pragma once

#include <core/helper/color.hpp>

#include "manager/settings.hpp"
#include "scenes/scene.hpp"
#include "ui/layouts/tile_layout.hpp"
#include "ui/widget.hpp"

#include <variant>

namespace details::settings::menu {

    enum class ReturnType : u8 { Save, Cancel };

    struct Return {
        ReturnType type;
    };

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
        enum class Status : u8 { Loading, Ok, Error };

        std::optional<details::settings::menu::Command> m_next_command{ std::nullopt };
        ui::TileLayout m_main_layout;
        //todo migrate to settings state
        std::vector<Color> m_colors;
        std::optional<std::shared_ptr<input::GameInput>> m_game_input;

        settings::Settings m_settings;
        bool m_did_change_settings{ false };
        Status m_status{ Status::Ok };

        const std::string listener_name = "settings_menu";

        explicit SettingsMenu(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::optional<std::shared_ptr<input::GameInput>>& game_input
        );

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit SettingsMenu(ServiceProvider* service_provider, const ui::Layout& layout);
        OOPETRIS_GRAPHICS_EXPORTED explicit SettingsMenu(
                ServiceProvider* service_provider,
                const ui::Layout& layout,
                const std::shared_ptr<input::GameInput>& game_input
        );

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED UpdateResult update() override;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) override;

        OOPETRIS_GRAPHICS_EXPORTED bool
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED void on_unhover() override;
    };

} // namespace scenes
