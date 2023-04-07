#include "application.hpp"

Application::Application(const std::string& title, WindowPosition position, int width, int height)
    : m_window{ title, position, width, height },
      m_renderer{ m_window } { }

Application::Application(const std::string& title, int x, int y, int width, int height)
    : m_window{ title, x, y, width, height },
      m_renderer{ m_window } { }

void Application::run(const u64 simulation_steps_per_second) {
    const auto start_time = elapsed_time();
    auto num_steps_simulated = u64{ 0 };
    m_event_dispatcher.register_listener(this);
    while (m_is_running) {
        const auto elapsed_since_start = elapsed_time() - start_time;
        const auto target_num_simulation_steps =
                static_cast<u64>(elapsed_since_start * static_cast<double>(simulation_steps_per_second));
        while (num_steps_simulated < target_num_simulation_steps) {
            m_event_dispatcher.dispatch_pending_events();
            update();
            ++num_steps_simulated;
        }
        render();
        m_renderer.present();

        const auto time_of_next_simulation_step =
                (static_cast<double>(num_steps_simulated + 1) * 1.0 / static_cast<double>(simulation_steps_per_second));
        const auto time_until_next_simulation_step = time_of_next_simulation_step - elapsed_time();
        if (time_until_next_simulation_step > 0.0) {
            SDL_Delay(static_cast<Uint32>(time_until_next_simulation_step * 1000.0));
        }
    }
}

void Application::handle_event(const SDL_Event& event) {
    if (event.type == SDL_QUIT or (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_ESCAPE)) {
        m_is_running = false;
    }
}

void Application::render() const {
    renderer().clear();
}
