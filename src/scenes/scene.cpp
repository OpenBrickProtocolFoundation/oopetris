#include "scene.hpp"
#include "game_over/game_over.hpp"
#include "ingame/ingame.hpp"
#include "main_menu/main_menu.hpp"
#include "pause/pause.hpp"
#include "settings_menu/settings_menu.hpp"

namespace scenes {
    Scene::Scene(ServiceProvider* service_provider, const ui::Layout& layout)
        : m_service_provider{ service_provider },
          layout{ layout } { }

    [[nodiscard]] std::unique_ptr<Scene>
    create_scene(ServiceProvider& service_provider, SceneId identifier, const ui::Layout& layout) {
        switch (identifier) {
            case SceneId::MainMenu:
                return std::make_unique<MainMenu>(&service_provider, layout);
            case SceneId::Pause:
                return std::make_unique<Pause>(&service_provider, layout);
            case SceneId::Ingame:
                return std::make_unique<Ingame>(&service_provider, layout);
            case SceneId::GameOver:
                return std::make_unique<GameOver>(&service_provider, layout);
            case SceneId::SettingsMenu:
                return std::make_unique<SettingsMenu>(&service_provider, layout);
            default:
                utils::unreachable();
        }
    }

    [[nodiscard]] ui::Layout Scene::get_layout() const {
        return layout;
    }

} // namespace scenes
