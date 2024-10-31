#pragma once

#include <core/helper/color.hpp>

#include "helper/windows.hpp"
#include "manager/font.hpp"
#include "rect.hpp"
#include "texture.hpp"
#include "window.hpp"

#include <SDL.h>
#include <filesystem>
#include <string>

struct Renderer final {
public:
    enum class VSync : u8 {
        Enabled,
        Disabled,
    };

private:
    SDL_Renderer* m_renderer;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit Renderer(const Window& window, VSync v_sync);
    OOPETRIS_GRAPHICS_EXPORTED Renderer(const Renderer&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED Renderer& operator=(const Renderer&) = delete;
    OOPETRIS_GRAPHICS_EXPORTED ~Renderer();

    OOPETRIS_GRAPHICS_EXPORTED void set_draw_color(const Color& color) const;
    OOPETRIS_GRAPHICS_EXPORTED void clear(const Color& clear_color = Color::black()) const;

    template<typename T>
    void draw_rect_filled(const shapes::AbstractRect<T>& rect, const Color& color) const {
        set_draw_color(color);
        const SDL_Rect sdl_rect = rect.to_sdl_rect();
        SDL_RenderFillRect(m_renderer, &sdl_rect);
    }

    template<typename T>
    void draw_rect_outline(const shapes::AbstractRect<T>& rect, const Color& color) const {
        set_draw_color(color);
        const SDL_Rect sdl_rect = rect.to_sdl_rect();
        SDL_RenderDrawRect(m_renderer, &sdl_rect);
    }

    template<typename S, typename T>
    void draw_line(const shapes::AbstractPoint<S>& start, const shapes::AbstractPoint<T>& end, const Color& color)
            const {
        set_draw_color(color);
        SDL_RenderDrawLine(
                m_renderer, static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x),
                static_cast<int>(end.y)
        );
    }

    template<typename S>
    void draw_pixel(const shapes::AbstractPoint<S>& location, const Color& color) const {
        set_draw_color(color);
        SDL_RenderDrawPoint(m_renderer, static_cast<int>(location.x), static_cast<int>(location.y));
    }

    template<typename S>
    void draw_self_computed_circle(const shapes::AbstractPoint<S>& center, S diameter, const Color& color) const {
        set_draw_color(color);
        draw_self_computed_circle_impl(center.template cast<i32>(), static_cast<i32>(diameter));
    }

    template<typename T>
    void draw_texture(const Texture& texture, const shapes::AbstractRect<T>& rect) const {
        texture.render(m_renderer, rect);
    }

    template<typename S, typename T>
    void draw_texture(const Texture& texture, const shapes::AbstractRect<S>& from, const shapes::AbstractRect<T>& dest)
            const {
        texture.render(m_renderer, from, dest);
    }

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Texture load_image(const std::filesystem::path& image_path) const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Texture prerender_text(
            const std::string& text,
            const Font& font,
            const Color& color,
            RenderType render_type = RenderType::Blended,
            const Color& background_color = Color::black()
    ) const;
    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Texture get_texture_for_render_target(const shapes::UPoint& size) const;

    OOPETRIS_GRAPHICS_EXPORTED void set_render_target(const Texture& texture) const;
    OOPETRIS_GRAPHICS_EXPORTED void reset_render_target() const;

    OOPETRIS_GRAPHICS_EXPORTED void present() const;

private:
    void draw_self_computed_circle_impl(const shapes::IPoint& center, i32 diameter) const;
};
