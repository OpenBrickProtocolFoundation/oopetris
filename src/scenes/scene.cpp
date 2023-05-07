#include "scene.hpp"
#include "ingame/ingame.hpp"
#include "pause/pause.hpp"

namespace scenes {
    Scene::Scene(ServiceProvider* service_provider) : m_service_provider{ service_provider } { }

    [[nodiscard]] std::unique_ptr<Scene> create_scene(ServiceProvider& service_provider, SceneId id) {
        switch (id) {
            case SceneId::MainMenu:
                return std::make_unique<Pause>(&service_provider);
            case SceneId::Ingame:
                return std::make_unique<Ingame>(&service_provider);
            default:
                utils::unreachable();
        }
    }

} // namespace scenes
