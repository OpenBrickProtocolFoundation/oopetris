#include "replay_game.hpp"
#include "../single_player_game/game_over.hpp"
#include "../single_player_game/pause.hpp"
#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"
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

        auto [parameters, information] = input::get_game_parameters_for_replay(service_provider, recording_path);


        std::vector<ui::Layout> layouts{};
        layouts.reserve(parameters.size());

        if (parameters.empty()) {
            throw std::runtime_error("An empty recording file isn't supported");
        } else if (parameters.size() == 1) { // NOLINT(readability-else-after-return,llvm-else-after-return)
            layouts.push_back(ui::RelativeLayout{ layout, 0.02, 0.01, 0.96, 0.98 });
        } else if (parameters.size() == 2) {
            layouts.push_back(ui::RelativeLayout{ layout, 0.02, 0.01, 0.46, 0.98 });
            layouts.push_back(ui::RelativeLayout{ layout, 0.52, 0.01, 0.46, 0.98 });
        } else {

            //TODO(Totto): support bigger layouts than just 2
            throw std::runtime_error("At the moment only replays from up to two players are supported");
        }

        u32 simulation_frequency = constants::simulation_frequency;
        if (const auto stored_simulation_frequency = information.get_if<u32>("simulation_frequency");
            stored_simulation_frequency.has_value()) {
            simulation_frequency = stored_simulation_frequency.value();
        }


        for (decltype(parameters.size()) i = 0; i < parameters.size(); ++i) {
            auto [input, starting_parameters] = std::move(parameters.at(i));

            m_games.emplace_back(std::make_unique<Game>(
                    service_provider, std::move(input), starting_parameters, simulation_frequency, layouts.at(i), false
            ));
        }


#if defined(_HAVE_DISCORD_SDK)
        if (auto& discord_instance = service_provider->discord_instance(); discord_instance.has_value()) {

            discord_instance->set_activity(
                    DiscordActivityWrapper("Replaying a game", discord::ActivityType::Playing)
                            .set_large_image("Playing OOPetris", constants::discord::ArtAsset::Logo)
                            .set_start_timestamp(std::chrono::system_clock::now())
            );
        }

#endif


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
            //TODO(Totto): the game input we use here for the game over is not guarantteed to work, better wul dbe to get one for this system at the tart of this scene!
            return UpdateResult{
                SceneUpdate::StopUpdating,
                Scene::RawPush{ "GameOver",
                               std::make_unique<scenes::SinglePlayerGameOver>(
                                        m_service_provider, ui::FullScreenLayout{ m_service_provider->window() },
                               m_games.at(0)->game_input()
                                ) }
            };
        }

        if (m_next_scene.has_value()) {
            const auto next_scene = m_next_scene.value();
            m_next_scene = std::nullopt;
            for (auto& game : m_games) {
                game->set_paused(true);
            }

            switch (next_scene) {
                case NextScene::Pause:
                    //TODO(Totto): the game input we use here for the Pauseis not guarantteed to work, better wul dbe to get one for this system at the tart of this scene!
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::RawPush{ "Pause", std::make_unique<scenes::SinglePlayerPause>(
                                                         m_service_provider, ui::FullScreenLayout{ m_service_provider->window() },
                                       m_games.at(0)->game_input()
                                                 ) }
                    };
                case NextScene::Settings:
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::Push{ SceneId::SettingsMenu,
                                    ui::RelativeLayout{ m_service_provider->window(), 0.15, 0.15, 0.7, 0.7 } }
                    };
                default:
                    UNREACHABLE();
            }
        }
        return UpdateResult{ SceneUpdate::StopUpdating, std::nullopt };
    }

    void ReplayGame::render(const ServiceProvider& service_provider) {
        for (const auto& game : m_games) {
            game->render(service_provider);
        }
    }

    [[nodiscard]] bool
    ReplayGame::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {

        //TODO(Totto): add gameInput to this function
        //TODO(Totto): re-add pause scene
        UNUSED(input_manager);
        UNUSED(event);
        /*   if (utils::event_is_action(event, utils::CrossPlatformAction::Pause)) {

            for (auto& game : m_games) {
                if (game->is_game_finished()) {
                    continue;
                }
                game->set_paused(true);
            }

            m_next_scene = NextScene::Pause;
            return true;
        }


        if (utils::event_is_action(event, utils::CrossPlatformAction::OpenSettings)) {
            m_next_scene = NextScene::Settings;
            return true;
        } */


        return false;
    }


} // namespace scenes
