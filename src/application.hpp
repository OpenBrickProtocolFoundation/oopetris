#pragma once

#include "command_line_arguments.hpp"
#include "event_dispatcher.hpp"
#include "event_listener.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "sdl_context.hpp"
#include "types.hpp"
#include "window.hpp"
#include <memory>
#include <vector>

struct Application : public EventListener {
private:
    SdlContext context;
    Window m_window;
    Renderer m_renderer;
    bool m_is_running{ true };
    CommandLineArguments m_command_line_arguments;

protected:
    EventDispatcher m_event_dispatcher;

private:
    std::vector<std::unique_ptr<Scene>> m_scene_stack;

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

    [[nodiscard]] static double elapsed_time() {
        return static_cast<double>(SDL_GetTicks()) / 1000.0;
    }

    void handle_event(const SDL_Event& event) override;

    [[nodiscard]] const CommandLineArguments& command_line_arguments() const {
        return m_command_line_arguments;
    }

    [[nodiscard]] EventDispatcher& event_dispatcher() {
        return m_event_dispatcher;
    }

    [[nodiscard]] const EventDispatcher& event_dispatcher() const {
        return m_event_dispatcher;
    }

protected:
    virtual void update();
    virtual void render() const;

    void push_scene(std::unique_ptr<Scene> scene) {
        m_scene_stack.push_back(std::move(scene));
    }
};
