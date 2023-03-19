#pragma once

#ifndef __SDL2__FOLDER_MISSING__
#include "SDL2/SDL.h"
#else
#include "SDL.h"
#endif

#include "point.hpp"
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
    Window(const Window&) = delete;
    ~Window();
    Window& operator=(const Window&) = delete;

    Point size() const;

    SDL_Window* get_sdl_window() const;
};
