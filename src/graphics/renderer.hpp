#pragma once

#include "helper/color.hpp"
#include "manager/font.hpp"
#include "rect.hpp"
#include "texture.hpp"
#include "window.hpp"
#include <SDL.h>
#include <string>

struct Renderer final {
public:
    enum class VSync {
        Enabled,
        Disabled,
    };

private:
    SDL_Renderer* m_renderer;

public:
    explicit Renderer(Window& window, VSync v_sync);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();

    void set_draw_color(Color color) const;
    void clear(Color clear_color = Color::black()) const;
    void draw_rect_filled(Rect rect, Color color) const;
    void draw_rect_outline(Rect rect, Color color) const;
    void draw_line(Point start, Point end, Color color) const;
    void draw_texture(const Texture& texture, const Rect& rect) const;

    Texture load_image(const std::string& image_path) const;
    Texture prerender_text(const std::string& text, const Font& font, const Color color) const;

    void present() const;

    friend struct Text;
};
