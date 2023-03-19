#pragma once

#include "SDL.h"

#include "color.hpp"
#include "rect.hpp"
#include "window.hpp"

struct Renderer final {
private:
    SDL_Renderer* m_renderer;

public:
    explicit Renderer(Window& window);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();

    void set_draw_color(Color color) const;
    void clear(Color clear_color = Color::black()) const;
    void draw_rect(Rect rect, Color color) const;
    void draw_line(Point from, Point to, Color color) const;

    void present() const;
};
