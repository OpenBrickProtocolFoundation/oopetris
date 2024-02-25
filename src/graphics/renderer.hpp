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

    void set_draw_color(const Color& color) const;
    void clear(const Color& clear_color = Color::black()) const;
    void draw_rect_filled(const shapes::Rect& rect, const Color& color) const;
    void draw_rect_outline(const shapes::Rect& rect, const Color& color) const;
    void draw_line(const shapes::Point& start, const shapes::Point& end, const Color& color) const;
    void draw_texture(const Texture& texture, const shapes::Rect& rect) const;
    void draw_texture(const Texture& texture, const shapes::Rect& from, const shapes::Rect& to) const;


    [[nodiscard]] Texture load_image(const std::string& image_path) const;
    [[nodiscard]] Texture prerender_text(
            const std::string& text,
            const Font& font,
            const Color& color,
            RenderType render_type = RenderType::Blended,
            const Color& background_color = Color::black()
    ) const;
    [[nodiscard]] Texture get_texture_for_render_target(const shapes::Point& size) const;

    void set_render_target(const Texture& texture) const;
    void reset_render_target() const;


    void present() const;

    friend struct Text;
};
