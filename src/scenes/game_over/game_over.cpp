#include "game_over.hpp"
#include "graphics/renderer.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "platform/capabilities.hpp"
#include <fmt/format.h>

namespace scenes {

    GameOver::GameOver(ServiceProvider* service_provider, const ui::Layout& layout)
        : Scene{ service_provider, layout },
          text{
            service_provider,
              fmt::format(
                      "Game Over, Press {} to continue",
                      utils::action_description(utils::CrossPlatformAction::EXIT)
              ),
               service_provider->fonts().get(FontId::Default),
              Color::white(),
              utils::device_orientation() == utils::Orientation::Landscape
                                       ? std::pair<double, double>{ 0.7, 0.07 }
                                       : std::pair<double, double>{ 0.95, 0.07 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                layout
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
                Scene::Switch{SceneId::MainMenu, ui::FullScreenLayout{ m_service_provider->window() }}
            };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void GameOver::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black(180));
        text.render(service_provider);
    }

    bool GameOver::handle_event(const SDL_Event& event, const Window*) {
        if (utils::event_is_action(event, utils::CrossPlatformAction::EXIT)) {
            m_should_exit = true;
            return true;
        }
        return false;
    }
} // namespace scenes
