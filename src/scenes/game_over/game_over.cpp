#include "game_over.hpp"
#include "graphics/renderer.hpp"
#include "helper/music_utils.hpp"
#include "helper/platform.hpp"
#include "input/input.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"

#include <fmt/format.h>

namespace scenes {

    GameOver::GameOver(ServiceProvider* service_provider, const ui::Layout& layout)
        : Scene{ service_provider, layout },
          m_text{
            service_provider,
              fmt::format(
                      "Game Over, Press {} to continue",
                     service_provider->input_manager().get_primary_input()->describe_navigation_event(input::NavigationEvent::BACK)
              ),
               service_provider->font_manager().get(FontId::Default),
              Color::white(),
              utils::get_orientation() == utils::Orientation::Landscape
                                       ? std::pair<double, double>{ 0.7, 0.07 }
                                       : std::pair<double, double>{ 0.95, 0.07 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                layout,
                true
          } {
        service_provider->music_manager()
                .load_and_play_music(
                        utils::get_assets_folder() / "music" / utils::get_supported_music_extension("05. Results")
                )
                .and_then(utils::log_error);
    }

    [[nodiscard]] Scene::UpdateResult GameOver::update() {
        if (m_should_exit) {
            return UpdateResult{
                SceneUpdate::StopUpdating,
                Scene::Switch{ SceneId::MainMenu, ui::FullScreenLayout{ m_service_provider->window() } }
            };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void GameOver::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black(180));
        m_text.render(service_provider);
    }

    bool GameOver::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {

        const auto navigation_event = input_manager->get_navigation_event(event);

        if (navigation_event == input::NavigationEvent::BACK) {
            m_should_exit = true;
            return true;
        }
        return false;
    }
} // namespace scenes
