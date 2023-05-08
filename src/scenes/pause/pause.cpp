#include "pause.hpp"
#include "../../renderer.hpp"
#include "../../resource_manager.hpp"

namespace scenes {

    Pause::Pause(ServiceProvider* service_provider)
    : Scene{ service_provider },
      m_heading{
          "Pause (ESC: continue, Return: quit)",
          Color::white(),
          service_provider->fonts().get(FontId::Default),
          ui::AbsoluteLayout{ 50, 50 }
      } { }

    [[nodiscard]] Scene::UpdateResult scenes::Pause::update() {
        if (m_should_unpause) {
            return std::pair{ scenes::SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        if (m_should_exit) {
            return std::pair{ scenes::SceneUpdate::StopUpdating, Scene::Exit{} };
        }
        return std::pair{ scenes::SceneUpdate::StopUpdating, tl::nullopt };
    }

    void Pause::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black(180));
        m_heading.render(service_provider, service_provider.window().screen_rect());
    }

    [[nodiscard]] bool Pause::handle_event(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    m_should_unpause = true;
                    return true;
                case SDLK_RETURN:
                    m_should_exit = true;
                    return true;
            }
        }
        return false;
    }

} // namespace scenes
