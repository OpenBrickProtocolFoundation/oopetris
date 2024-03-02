#include "scenes/scene.hpp"
#include "about_page/about_page.hpp"
#include "ai_game/ai_game.hpp"
#include "game_over/game_over.hpp"
#include "main_menu/main_menu.hpp"
#include "multiplayer_menu/multiplayer_menu.hpp"
#include "pause/pause.hpp"
#include "play_select_menu/play_select_menu.hpp"
#include "settings_menu/settings_menu.hpp"
#include "single_player_game/single_player_game.hpp"

#if !defined(_ONLINE_MULTIPLAYER_NOT_SUPPORTED)
#include "online_lobby/online_lobby.hpp"
#endif


namespace scenes {
    Scene::Scene(ServiceProvider* service_provider, const ui::Layout& layout)
        : m_service_provider{ service_provider },
          m_layout{ layout } { }

    [[nodiscard]] std::unique_ptr<Scene>
    create_scene(ServiceProvider& service_provider, SceneId identifier, const ui::Layout& layout) {
        switch (identifier) {
            case SceneId::MainMenu:
                return std::make_unique<MainMenu>(&service_provider, layout);
            case SceneId::Pause:
                return std::make_unique<Pause>(&service_provider, layout);
            case SceneId::SinglePlayerGame:
                return std::make_unique<SinglePlayerGame>(&service_provider, layout);
            case SceneId::GameOver:
                return std::make_unique<GameOver>(&service_provider, layout);
            case SceneId::SettingsMenu:
                return std::make_unique<SettingsMenu>(&service_provider, layout);
            case SceneId::AboutPage:
                return std::make_unique<AboutPage>(&service_provider, layout);
            case SceneId::PlaySelectMenu:
                return std::make_unique<PlaySelectMenu>(&service_provider, layout);
            case SceneId::MultiPlayerModeSelectMenu:
                return std::make_unique<MultiPlayerMenu>(&service_provider, layout);
#if !defined(_ONLINE_MULTIPLAYER_NOT_SUPPORTED)
            case SceneId::OnlineLobby:
                return std::make_unique<OnlineLobby>(&service_provider, layout);
#endif
            case SceneId::AIMultiPlayerGame:
                return std::make_unique<AIGame>(&service_provider, layout);
            //TODO
            /*      
            case SceneId::LocalMultiPlayerGame:
                return std::make_unique<TODO>(&service_provider, layout);
            case SceneId::OnlineMultiplayerGame:
                return std::make_unique<TODO>(&service_provider, layout);
            case SceneId::AchievementsPage:
                return std::make_unique<TODO>(&service_provider, layout); */
            default:
                utils::unreachable();
        }
    }

    void Scene::on_unhover() { }

    [[nodiscard]] const ui::Layout& Scene::get_layout() const {
        return m_layout;
    }

} // namespace scenes
