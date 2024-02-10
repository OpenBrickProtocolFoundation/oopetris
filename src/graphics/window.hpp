#pragma once

#include "point.hpp"
#include "rect.hpp"
#include <SDL.h>
#include <string>

enum class WindowPosition {
    Centered = SDL_WINDOWPOS_CENTERED,
    Undefined = SDL_WINDOWPOS_UNDEFINED,
};

struct Window final {
private:
    SDL_Window* m_window;

public:
    Window(const std::string& title, WindowPosition position, int width, int height);
    Window(const std::string& title, int x, int y, int width, int height);
    Window(const std::string& title, WindowPosition position);
    Window(const std::string& title, int x, int y);
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;
    ~Window();

    [[nodiscard]] Point size() const;

    [[nodiscard]] SDL_Window* get_sdl_window() const;

    [[nodiscard]] Rect screen_rect() const;
};
