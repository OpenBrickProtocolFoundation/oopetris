#pragma once

#include "point.hpp"
#include "rect.hpp"
#include <SDL.h>
#include <cstdint>
#include <sdl_compatibility.h>
#include <string>

enum class WindowPosition : std::uint32_t {
    Centered = SDL_WINDOWPOS_CENTERED,
    Undefined = SDL_WINDOWPOS_UNDEFINED,
};

struct Window final {
private:
#ifdef _USE_SDL_LEGACY_VERSION
    SDL_Surface* m_screen;
#else
    SDL_Window* m_window;
#endif


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
#ifdef _USE_SDL_LEGACY_VERSION
    [[nodiscard]] SDL_Surface* get_sdl_screen() const;
#else
    [[nodiscard]] SDL_Window* get_sdl_window() const;
#endif


    [[nodiscard]] Rect screen_rect() const;
};
