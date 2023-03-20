#pragma once

#include "renderer.hpp"
#include "sdl_context.hpp"
#include "window.hpp"

struct Application {
private:
    SdlContext context;
    Window m_window;
    Renderer m_renderer;
    bool m_is_running{ true };

public:
    Application(const std::string& title, WindowPosition position, int width, int height);
    Application(const std::string& title, int x, int y, int width, int height);
    virtual ~Application() = default;
    void run(int target_frames_per_second);

    const Renderer& renderer() const {
        return m_renderer;
    }

    const Window& window() const {
        return m_window;
    }


    static double elapsed_time() {
        return static_cast<double>(SDL_GetTicks()) / 1000.0;
    }

protected:
    virtual bool handle_event(const SDL_Event& event);
    virtual void update(double delta_time) = 0;
    virtual void render() const;
};
