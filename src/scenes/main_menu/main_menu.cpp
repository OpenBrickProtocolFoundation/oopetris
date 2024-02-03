#include "main_menu.hpp"
#include "../../constants.hpp"
#include "../../music_manager.hpp"
#include "../../resource_manager.hpp"
#include "../../ui/layout.hpp"
#include "../../window.hpp"

namespace scenes {

    MainMenu::MainMenu(ServiceProvider* service_provider)
        : Scene{ SceneId::MainMenu, service_provider },
          m_main_grid{ ui::FullScreenLayout{ service_provider->window() },ui::Direction::Vertical, ui::RelativeMargin{service_provider->window(),ui::Direction::Vertical, 0.05}, std::pair<double, double>{ 0.05, 0.05 } } {

        m_main_grid.add<ui::Label>(
                0, constants::program_name, Color::white(), service_provider->fonts().get(FontId::Default),
                std::pair<double, double>{ 0.4, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        constexpr auto button_size = std::pair<double, double>{ 1.0, 1.0 };
        constexpr auto button_alignment =
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center };
        constexpr auto button_margins = std::pair<double, double>{ 0.05, 0.05 };

        m_main_grid.add<ui::Button>(
                1, "Start", 0,
                [this](const ui::Button&) {
                    spdlog::info("setting next command");
                    m_next_command = Command::StartGame;
                },
                button_size, button_alignment, button_margins
        );

        m_main_grid.add<ui::Button>(
                2, "Settings", 50,
                [this](const ui::Button&) {
                    spdlog::info("setting next command");
                    m_next_command = Command::OpenSettingsMenu;
                },
                button_size, button_alignment, button_margins
        );

        m_main_grid.add<ui::Button>(
                3, "Exit", 100, [this](const ui::Button&) { m_next_command = Command::Exit; }, button_size,
                button_alignment, button_margins
        );

        service_provider->music_manager()
                .load_and_play_music(
                        utils::get_assets_folder() / "music" / utils::get_supported_music_extension("01. Main Menu")
                )
                .and_then(utils::log_error);
    }

    [[nodiscard]] Scene::UpdateResult MainMenu::update() {
        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::StartGame:
                    return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Switch{ SceneId::Ingame } };
                case Command::OpenSettingsMenu:
                    // perform a push and reset the command, so that the music keeps playing the entire time
                    m_next_command = tl::nullopt;
                    return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Push{ SceneId::SettingsMenu } };
                case Command::Exit:
                    return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Exit{} };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::ContinueUpdating, tl::nullopt };
    }

    void MainMenu::render(const ServiceProvider& service_provider) {
        m_main_grid.render(service_provider);
    }

    bool MainMenu::handle_event(const SDL_Event& event) {
        if (m_main_grid.handle_event(event)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_next_command = Command::Exit;
            return true;
        }
        return false;
    }

} // namespace scenes
