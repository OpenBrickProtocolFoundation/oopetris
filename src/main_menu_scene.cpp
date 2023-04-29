#include "main_menu_scene.hpp"
#include "tetris_application.hpp"

MainMenuScene::MainMenuScene(TetrisApplication* app)
    : Scene{app},
      m_heading{ "Test", Color::white(), app->fonts().get("default"), ui::AbsoluteLayout{50, 50 } } {
    app->event_dispatcher().register_listener(this);
}

MainMenuScene::~MainMenuScene() {
    m_application->event_dispatcher().unregister_listener(this);
}

[[nodiscard]] Scene::UpdateResult MainMenuScene::update() {
    if (m_should_end) {
        return std::pair{ SceneUpdate::StopUpdating, Scene::Pop{} };
    }
    return std::pair{ SceneUpdate::StopUpdating, tl::nullopt };
}

void MainMenuScene::render(const Application& app) {
    m_heading.render(app, app.window().screen_rect());
}
void MainMenuScene::handle_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_m) {
        spdlog::info("m key pressed");
        m_should_end = true;
    }
}
