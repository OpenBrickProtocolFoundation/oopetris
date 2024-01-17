#pragma once

#include "color.hpp"
#include "font.hpp"
#include "rect.hpp"
#include "window.hpp"
#include <SDL.h>
#include <sdl_compatibility.h>
#include <string>

struct Renderer final {
public:
    enum class VSync {
        Enabled,
        Disabled,
    };

private:
#ifdef _USE_SDL_LEGACY_VERSION
    Window* m_window;
#else
    SDL_Renderer* m_renderer;
#endif

public:
#ifdef _USE_SDL_LEGACY_VERSION
    explicit Renderer(Window* window);
#else

    explicit Renderer(Window& window, VSync v_sync);
#endif


    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();

#ifndef _USE_SDL_LEGACY_VERSION
    void set_draw_color(Color color) const;
#endif
    void clear(Color clear_color = Color::black()) const;
    void draw_rect_filled(Rect rect, Color color) const;
    void draw_rect_outline(Rect rect, Color color) const;
    void draw_line(Point from, Point to, Color color) const;
    void draw_text(Point position, const std::string& text, const Font& font, Color color) const;

    void present() const;

    friend struct Text;
};
