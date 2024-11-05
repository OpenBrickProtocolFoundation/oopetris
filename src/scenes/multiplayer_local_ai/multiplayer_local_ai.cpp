#include <core/helper/date.hpp>
#include <core/helper/errors.hpp>
#include <core/helper/magic_enum_wrapper.hpp>


#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"
#include "helper/music_utils.hpp"
#include "helper/platform.hpp"
#include "manager/music_manager.hpp"
#include "multiplayer_local_ai.hpp"
#include "scenes/settings_menu/settings_menu.hpp"
#include "scenes/single_player_game/pause.hpp"

namespace scenes {

    MultiPlayerLocalAIGame::MultiPlayerLocalAIGame(
            ServiceProvider* service_provider,
            const ui::Layout& layout,
            std::unique_ptr<oopetris::AI>&& input_ai
    )
        : Scene{ service_provider, layout },
          m_ai{ std::move(input_ai) } {

        const auto date = date::ISO8601Date::now();
        const auto simulation_frequency = constants::simulation_frequency;

        recorder::AdditionalInformation additional_information{};
        additional_information.add<u32>("simulation_frequency", simulation_frequency);
        additional_information.add("mode", "multiplayer_local_ai");
        additional_information.add("ai_name", m_ai->name());
        additional_information.add("platform", std::string{ magic_enum::enum_name(utils::get_platform()) });
        additional_information.add("date", date.value());
        //TODO(Totto): add more information, if logged in

        auto result = input::get_single_player_game_parameters_for_multiplayer(
                service_provider, std::move(additional_information), date, 1
        );

        if (not result.has_value()) {
            throw helper::MajorError(fmt::format(
                    "No suitable input was configured, go into the settings to select a suitable input: {}",
                    result.error()
            ));
        }

        auto [input, starting_parameters] = result.value();

        assert(starting_parameters.size() == 2);


        m_my_game = std::make_unique<Game>(
                service_provider, std::move(input), starting_parameters.at(0), simulation_frequency,
                ui::RelativeLayout{ layout, 0.02, 0.01, 0.46, 0.98 }, false
        );

        m_ai_game = std::make_unique<Game>(
                service_provider, m_ai->input(), starting_parameters.at(1), simulation_frequency,
                ui::RelativeLayout{ layout, 0.52, 0.01, 0.46, 0.98 }, false
        );


#if defined(_HAVE_DISCORD_SDK)
        if (auto& discord_instance = service_provider->discord_instance(); discord_instance.has_value()) {

            discord_instance->set_activity(
                    DiscordActivityWrapper(
                            "Playing a local multi-player game against AI", discord::ActivityType::Playing
                    )
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


    [[nodiscard]] Scene::UpdateResult MultiPlayerLocalAIGame::update() {
        /* if (m_game->is_game_finished()) {
        return UpdateResult{
            SceneUpdate::StopUpdating,
            Scene::RawPush{ "GameOver",
                            std::make_unique<scenes::SinglePlayerGame>(
                                    m_service_provider, ui::FullScreenLayout{ m_service_provider->window() },
                            m_game->game_input()
                            ) }
        };
    } */

        m_my_game->update();
        m_ai_game->update();


        if (m_next_scene.has_value()) {
            const auto next_scene = m_next_scene.value();
            m_next_scene = std::nullopt;
            m_my_game->set_paused(true);
            m_ai_game->set_paused(true);

            switch (next_scene) {
                case NextScene::Pause:
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::RawPush{ "Pause", std::make_unique<scenes::SinglePlayerPause>(
                                                         m_service_provider, ui::FullScreenLayout{ m_service_provider->window() },
                                       m_my_game->game_input()
                                                 ) }
                    };
                case NextScene::Settings:
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::RawPush{ "SettingsMenu", std::make_unique<SettingsMenu>(
                                                                m_service_provider, ui::RelativeLayout{ m_service_provider->window(), 0.15,
                                                                                    0.15, 0.7, 0.7 },
                                       m_my_game->game_input()
                                                        )

                        }
                    };
                default:
                    UNREACHABLE();
            }
        }
        return UpdateResult{ SceneUpdate::StopUpdating, std::nullopt };
    }

    void MultiPlayerLocalAIGame::render(const ServiceProvider& service_provider) {
        m_my_game->render(service_provider);
        m_ai_game->render(service_provider);
    }

    [[nodiscard]] bool MultiPlayerLocalAIGame::handle_event(
            const std::shared_ptr<input::InputManager>& input_manager,
            const SDL_Event& event
    ) {

        const auto& game_input = m_my_game->game_input();

        const auto& menu_event = game_input->get_menu_event(event);

        if ((menu_event == input::MenuEvent::Pause
             or input_manager->get_navigation_event(event) == input::NavigationEvent::BACK)
            and not m_my_game->is_game_finished()) {
            m_next_scene = NextScene::Pause;
            m_my_game->set_paused(true);
            m_ai_game->set_paused(true);
            return true;
        }

        if (menu_event == input::MenuEvent::OpenSettings) {
            m_next_scene = NextScene::Settings;
            return true;
        }

        return false;
    }


} // namespace scenes
