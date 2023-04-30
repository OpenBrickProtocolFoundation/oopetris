#include "scene.hpp"
#include "ingame_scene.hpp"
#include "main_menu_scene.hpp"

Scene::Scene(ServiceProvider* service_provider) : m_service_provider{ service_provider } { }

[[nodiscard]] std::unique_ptr<Scene> create_scene(ServiceProvider& service_provider, SceneId id) {
    switch (id) {
        case SceneId::MainMenu:
            return std::make_unique<MainMenuScene>(&service_provider);
        case SceneId::Ingame:
            return std::make_unique<IngameScene>(&service_provider);
        default:
            assert(false and "unreachable");
            return {};
    }
}
