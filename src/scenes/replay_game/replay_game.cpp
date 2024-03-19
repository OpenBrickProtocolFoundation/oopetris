#include "replay_game.hpp"
#include "helper/music_utils.hpp"
#include "manager/music_manager.hpp"
#include "scenes/scene.hpp"

#include <vector>

namespace scenes {

    ReplayGame::ReplayGame(
            ServiceProvider* service_provider,
            const ui::Layout& layout,
            const std::filesystem::path& recording_path
    )
        : Scene{ service_provider, layout } {

        auto parameters = input::get_game_parameters_for_replay(service_provider, recording_path);


        std::vector<ui::Layout> layouts{};
        layouts.reserve(parameters.size());

        if (parameters.empty()) {
            throw std::runtime_error("An empty recording file isn't supported");
        } else if (parameters.size() == 1) { // NOLINT(readability-else-after-return)
            layouts.push_back(ui::RelativeLayout{ layout, 0.02, 0.01, 0.96, 0.98 });
        } else if (parameters.size() == 2) {
            layouts.push_back(ui::RelativeLayout{ layout, 0.02, 0.01, 0.46, 0.98 });
            layouts.push_back(ui::RelativeLayout{ layout, 0.52, 0.01, 0.46, 0.98 });
        } else {

            //TODO: support bigger layouts than just 2
            throw std::runtime_error("At the moment only replays from up to two players are supported");
        }

        for (decltype(parameters.size()) i = 0; i < parameters.size(); ++i) {
            auto [input, starting_parameters] = std::move(parameters.at(i));

            m_games.emplace_back(std::make_unique<Game>(
                    service_provider, std::move(input), starting_parameters, layouts.at(i), false
            ));
        }


        m_service_provider->music_manager()
                .load_and_play_music(
                        utils::get_assets_folder() / "music" / utils::get_supported_music_extension("02. Game Theme")
                )
                .and_then(utils::log_error);
    }


    [[nodiscard]] Scene::UpdateResult ReplayGame::update() {
        bool all_games_finished = true;
        for (auto& game : m_games) {
            if (game->is_game_finished()) {
                continue;
            }

            all_games_finished = false;
            game->update();
        }

        if (all_games_finished) {
            return UpdateResult{
                SceneUpdate::StopUpdating,
                Scene::Push{SceneId::GameOver, ui::FullScreenLayout{ m_service_provider->window() }}
            };
        }

        if (m_next_scene.has_value()) {
            const auto next_scene = m_next_scene.value();
            m_next_scene = helper::nullopt;
            for (auto& game : m_games) {
                game->set_paused(true);
            }

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

    void ReplayGame::render(const ServiceProvider& service_provider) {
        for (const auto& game : m_games) {
            game->render(service_provider);
        }
    }

    [[nodiscard]] bool ReplayGame::handle_event(const SDL_Event& event, const Window*) {

        if (utils::event_is_action(event, utils::CrossPlatformAction::PAUSE)) {

            for (auto& game : m_games) {
                if (game->is_game_finished()) {
                    continue;
                }
                game->set_paused(true);
            }

            m_next_scene = NextScene::Pause;
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
