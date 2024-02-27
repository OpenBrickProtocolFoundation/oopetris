#include "single_player_game.hpp"
#include "game/input_creator.hpp"
#include "scenes/scene.hpp"
namespace scenes {

    SinglePlayerGame::SinglePlayerGame(ServiceProvider* service_provider, const ui::Layout& layout)
        : Scene{ service_provider, layout } {


        auto parameters = input::get_game_parameters(service_provider, 1);

        auto [input, starting_parameters] = std::move(parameters.at(0));

        m_game = std::make_unique<Game>(service_provider, layout, std::move(input), starting_parameters);

        m_service_provider->music_manager()
                .load_and_play_music(
                        utils::get_assets_folder() / "music" / utils::get_supported_music_extension("02. Game Theme")
                )
                .and_then(utils::log_error);
    }


    [[nodiscard]] Scene::UpdateResult SinglePlayerGame::update() {
        if (m_game->is_game_over()) {
            return UpdateResult{
                SceneUpdate::StopUpdating,
                Scene::Push{SceneId::GameOver, ui::FullScreenLayout{ m_service_provider->window() }}
            };
        }

        m_game->update();

        if (m_next_scene.has_value()) {
            const auto next_scene = m_next_scene.value();
            m_next_scene = helper::nullopt;
            m_game->set_paused(true);

            switch (next_scene) {
                case NextScene::Pause:
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::Push{SceneId::Pause, ui::FullScreenLayout{ m_service_provider->window() }}
                    };
                case NextScene::Settings:
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::Push{SceneId::SettingsMenu,
                                    ui::RelativeLayout{ m_service_provider->window(), 0.15, 0.15, 0.7, 0.7 }}
                    };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void SinglePlayerGame::render(const ServiceProvider& service_provider) {
        m_game->render(service_provider);
    }

    [[nodiscard]] bool SinglePlayerGame::handle_event(const SDL_Event& event, const Window*) {

        if (utils::event_is_action(event, utils::CrossPlatformAction::PAUSE) and not m_game->is_game_over()) {
            m_next_scene = NextScene::Pause;
            m_game->set_paused(true);
            return true;
        }

        if (utils::device_supports_keys()) {
            if (utils::event_is_action(event, utils::CrossPlatformAction::OPEN_SETTINGS)) {
                m_next_scene = NextScene::Settings;
                return true;
            }
        }
        return false;
    }


} // namespace scenes
