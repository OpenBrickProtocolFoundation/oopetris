#pragma once

#include <core/helper/point.hpp>

#include "graphics/rect.hpp"
#include "helper/message_box.hpp"
#include "helper/windows.hpp"
#include "sdl_context.hpp"

#include <SDL.h>
#include <string>

enum class WindowPosition {
    Centered = SDL_WINDOWPOS_CENTERED,
    Undefined = SDL_WINDOWPOS_UNDEFINED,
};

struct Window final {
private:
    SDL_Window* m_window;
    SdlContext m_context;

public:
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, WindowPosition position, u32 width, u32 height);
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, u32 x, u32 y, u32 width, u32 height);
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, WindowPosition position);
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, u32 x, u32 y);
    OOPETRIS_GRAPHICS_EXPORTED Window(const Window&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED Window(Window&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED Window& operator=(const Window&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED Window& operator=(Window&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED ~Window();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] shapes::UPoint size() const;

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] SDL_Window* get_sdl_window() const;

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const SdlContext& context() const;

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] shapes::URect screen_rect() const;

    OOPETRIS_GRAPHICS_EXPORTED void
    show_simple(helper::MessageBox::Type type, const std::string& title, const std::string& content) const;
};
