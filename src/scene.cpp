#include "scene.hpp"
#include "ingame_scene.hpp"
#include "main_menu_scene.hpp"
#include "tetris_application.hpp"

Scene::Scene(TetrisApplication* app) : m_application{ app } { }

[[nodiscard]] std::unique_ptr<Scene> create_scene(TetrisApplication& app, SceneId id) {
    switch (id) {
        case SceneId::MainMenu:
            return std::make_unique<MainMenuScene>(&app);
        case SceneId::Ingame:
            return std::make_unique<IngameScene>(&app);
        default:
            assert(false and "unreachable");
            return {};
    }
}
