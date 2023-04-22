#include "application.hpp"

#if defined(__SWITCH__)
#include "switch_buttons.hpp"
#include <switch.h>
#endif

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
    m_event_dispatcher.register_listener(this);
    while (m_is_running) {
        m_event_dispatcher.dispatch_pending_events();
        update();
        render();
        m_renderer.present();
    }
}

void Application::handle_event(const SDL_Event& event) {


#if defined(__SWITCH__)
    // exit with plus (M on most emulators)
    if (event.type == SDL_JOYBUTTONDOWN and event.jbutton.button == JOYCON_PLUS) {
        m_is_running = false;
    }
#endif

    if (event.type == SDL_QUIT or (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_ESCAPE)) {
        m_is_running = false;
    }

#if defined(__ANDROID__)
    if (event.type == SDL_KEYDOWN and event.key.keysym.sym == SDLK_AC_BACK) {
        //TODO: also catch the resume event from the app (see SDLActivity.java)
        // pause()
    }
#endif
}

void Application::render() const {
    renderer().clear();
}
