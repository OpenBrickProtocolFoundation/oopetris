#include "main_menu_scene.hpp"

MainMenuScene::MainMenuScene(ServiceProvider* service_provider)
    : Scene{ service_provider },
      m_heading{
          "Pause (ESC: continue, Return: quit)",
          Color::white(),
          service_provider->fonts().get(FontId::Default),
          ui::AbsoluteLayout{ 50, 50 }
      } { }

[[nodiscard]] Scene::UpdateResult MainMenuScene::update() {
    if (m_should_end) {
        return std::pair{ SceneUpdate::StopUpdating, Scene::Pop{} };
    }
    if (m_should_exit) {
        return std::pair{ SceneUpdate::StopUpdating, Scene::Exit{} };
    }
    return std::pair{ SceneUpdate::StopUpdating, tl::nullopt };
}

void MainMenuScene::render(const Application& app) {
    m_heading.render(app, app.window().screen_rect());
}

[[nodiscard]] bool MainMenuScene::handle_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                m_should_end = true;
                return true;
            case SDLK_RETURN:
                m_should_exit = true;
                return true;
        }
    }
    return false;
}
