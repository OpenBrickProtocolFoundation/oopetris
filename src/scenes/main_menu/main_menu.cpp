#include "main_menu.hpp"
#include "../../constants.hpp"
#include "../../resource_manager.hpp"
#include "../../window.hpp"

namespace scenes {

    MainMenu::MainMenu(ServiceProvider* service_provider)
        : Scene(service_provider),
          m_heading{
              constants::program_name, Color::white(), service_provider->fonts().get(FontId::Default),
              ui::AbsoluteLayout{100, 100}
    },
          m_focus_group{ ui::AbsoluteLayout{ 0, 0 } } {
        auto button = std::make_unique<ui::Button>(ui::AbsoluteLayout{ 100, 200 }, 0, [this](const ui::Button&) {
            spdlog::info("setting next command");
            m_next_command = Command::StartGame;
        });
        m_focus_group.add(std::move(button));
    }

    [[nodiscard]] Scene::UpdateResult MainMenu::update() {
        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::StartGame:
                    return UpdateResult{
                        SceneUpdate::ContinueUpdating, Scene::Switch{SceneId::Ingame, false}
                    };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::ContinueUpdating, tl::nullopt };
    }

    void MainMenu::render(const ServiceProvider& service_provider) {
        m_heading.render(service_provider, service_provider.window().screen_rect());
    }

    bool MainMenu::handle_event(const SDL_Event& event) {
        return m_focus_group.handle_event(event);
    }

} // namespace scenes
