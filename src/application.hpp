#pragma once

#include "command_line_arguments.hpp"
#include "event_dispatcher.hpp"
#include "event_listener.hpp"
#include "renderer.hpp"
#include "sdl_context.hpp"
#include "types.hpp"
#include "window.hpp"

struct Application : public EventListener {
private:
    SdlContext context;
    Window m_window;
    Renderer m_renderer;
    bool m_is_running{ true };
    static inline u64 s_num_steps_simulated{ 0 };
    CommandLineArguments m_command_line_arguments;

protected:
    EventDispatcher m_event_dispatcher;

public:
    Application(
            const std::string& title,
            WindowPosition position,
            int width,
            int height,
            CommandLineArguments command_line_arguments
    );

    Application(
            const std::string& title,
            int x,
            int y,
            int width,
            int height,
            CommandLineArguments command_line_arguments
    );

    Application(const std::string& title, WindowPosition position, CommandLineArguments command_line_arguments);

    Application(const std::string& title, int x, int y, CommandLineArguments command_line_arguments);

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

    [[nodiscard]] const Renderer& renderer() const {
        return m_renderer;
    }

    [[nodiscard]] const Window& window() const {
        return m_window;
    }

    static double elapsed_time() {
        return static_cast<double>(SDL_GetTicks()) / 1000.0;
    }

    static u64 simulation_step_index() {
        return s_num_steps_simulated;
    }

    void handle_event(const SDL_Event& event) override;

protected:
    virtual void update() = 0;
    virtual void render() const;

    [[nodiscard]] const CommandLineArguments& command_line_arguments() const {
        return m_command_line_arguments;
    }
};
