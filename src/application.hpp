#pragma once

#include "renderer.hpp"
#include "sdl_context.hpp"
#include "window.hpp"
#include "event_dispatcher.hpp"
#include "event_listener.hpp"

struct Application : public EventListener {
private:
    SdlContext context;
    Window m_window;
    Renderer m_renderer;
    bool m_is_running{ true };

protected:
    EventDispatcher m_event_dispatcher;

public:
    Application(const std::string& title, WindowPosition position, int width, int height);
    Application(const std::string& title, int x, int y, int width, int height);
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run(int target_frames_per_second);

    [[nodiscard]] const Renderer& renderer() const {
        return m_renderer;
    }

    [[nodiscard]] const Window& window() const {
        return m_window;
    }


    static double elapsed_time() {
        return static_cast<double>(SDL_GetTicks()) / 1000.0;
    }

    void handle_event(const SDL_Event& event) override;

protected:
    virtual void update(double delta_time) = 0;
    virtual void render() const;
};
