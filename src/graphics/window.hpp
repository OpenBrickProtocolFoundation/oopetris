#pragma once

#include <core/helper/point.hpp>

#include "graphics/rect.hpp"
#include "helper/export_symbols.hpp"
#include "helper/message_box.hpp"
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
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, u32 x_pos, u32 y_pos, u32 width, u32 height);
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, WindowPosition position);
    OOPETRIS_GRAPHICS_EXPORTED Window(const std::string& title, u32 x_pos, u32 y_pos);
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED ~Window();

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED shapes::UPoint size() const;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED SDL_Window* get_sdl_window() const;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED const SdlContext& context() const;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED shapes::URect screen_rect() const;

    OOPETRIS_GRAPHICS_EXPORTED void
    show_simple(helper::MessageBox::Type type, const std::string& title, const std::string& content) const;
};
