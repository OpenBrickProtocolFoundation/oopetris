#include "main_menu.hpp"
#include "../../constants.hpp"
#include "../../music_manager.hpp"
#include "../../resource_manager.hpp"
#include "../../window.hpp"

namespace scenes {

    MainMenu::MainMenu(ServiceProvider* service_provider, Window* window)
        : Scene(service_provider),
          m_heading{
              constants::program_name, Color::white(), service_provider->fonts().get(FontId::Default),
              ui::AbsoluteLayout{100, 100}
    },
          m_focus_group{ ui::AbsoluteLayout{ 0, 0 } } {
        m_focus_group.add(std::make_unique<ui::Button>(
                "Start", ui::AbsoluteLayout{ 100, 200 }, 0,
                [this](const ui::Button&) {
                    spdlog::info("setting next command");
                    m_next_command = Command::StartGame;
                },
                window
        ));
        m_focus_group.add(std::make_unique<ui::Button>(
                "Settings", ui::AbsoluteLayout{ 100, 250 }, 50,
                [this](const ui::Button&) {
                    spdlog::info("setting next command");
                    m_next_command = Command::OpenSettingsMenu;
                },
                window
        ));
        m_focus_group.add(std::make_unique<ui::Button>(
                "Exit", ui::AbsoluteLayout{ 100, 300 }, 100,
                [this](const ui::Button&) { m_next_command = Command::Exit; }, window
        ));

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
                    return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Switch{ SceneId::SettingsMenu } };
                case Command::Exit:
                    return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Exit{} };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::ContinueUpdating, tl::nullopt };
    }

    void MainMenu::render(const ServiceProvider& service_provider) {
        m_heading.render(service_provider, service_provider.window().screen_rect());
        m_focus_group.render(service_provider, service_provider.window().screen_rect());
    }

    bool MainMenu::handle_event(const SDL_Event& event) {
        return m_focus_group.handle_event(event);
    }

} // namespace scenes
