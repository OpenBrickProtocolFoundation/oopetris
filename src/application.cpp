#include "application.hpp"

Application::Application(const std::string& title, WindowPosition position, int width, int height)
    : m_window{ title, position, width, height },
      m_renderer{ m_window } { }

Application::Application(const std::string& title, int x, int y, int width, int height)
    : m_window{ title, x, y, width, height },
      m_renderer{ m_window } { }

void Application::run(int target_frames_per_second) {
    const double target_frame_duration = 1.0 / static_cast<double>(target_frames_per_second);
    Uint32 last_ticks = SDL_GetTicks();
    m_event_dispatcher.register_listener(this);
    while (m_is_running) {
        m_event_dispatcher.dispatch_pending_events();
        const Uint32 current_ticks = SDL_GetTicks();
        const double delta_time = static_cast<double>(current_ticks - last_ticks) / 1000.0;
        last_ticks = current_ticks;
        update(delta_time);
        render();
        m_renderer.present();

        if (delta_time < target_frame_duration) {
            SDL_Delay(static_cast<Uint32>((target_frame_duration - delta_time) * 1000.0));
        }
    }
}

void Application::handle_event(const SDL_Event& event) {
    if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
        m_is_running = false;
    }
}

void Application::render() const {
    renderer().clear();
}
