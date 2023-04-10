#include "application.hpp"

Application::Application(
        const std::string& title,
        WindowPosition position,
        int width,
        int height,
        CommandLineArguments command_line_arguments
)
    : m_window{ title, position, width, height },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

Application::Application(
        const std::string& title,
        int x,
        int y,
        int width,
        int height,
        CommandLineArguments command_line_arguments
)
    : m_window{ title, x, y, width, height },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

Application::Application(const std::string& title, WindowPosition position, CommandLineArguments command_line_arguments)
    : m_window{ title, position },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

Application::Application(
        const std::string& title,
        int x,
        int y,
        CommandLineArguments command_line_arguments
)
    : m_window{ title, x, y, },
      m_renderer{ m_window },
      m_command_line_arguments{ std::move(command_line_arguments) } { }

void Application::run() {
    const auto start_time = elapsed_time();
    s_num_steps_simulated = 0;
    m_event_dispatcher.register_listener(this);
    while (m_is_running) {
        const auto elapsed_since_start = elapsed_time() - start_time;
        const auto target_num_simulation_steps =
                static_cast<u64>(elapsed_since_start * static_cast<double>(m_command_line_arguments.target_fps));
        while (s_num_steps_simulated < target_num_simulation_steps) {
            m_event_dispatcher.dispatch_pending_events();
            update();
            ++s_num_steps_simulated;
        }
        render();
        m_renderer.present();

        const auto time_of_next_simulation_step =
                (static_cast<double>(s_num_steps_simulated + 1) * 1.0
                 / static_cast<double>(m_command_line_arguments.target_fps));
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
