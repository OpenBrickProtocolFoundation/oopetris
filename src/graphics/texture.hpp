
#pragma once


#include <core/helper/color.hpp>
#include <core/helper/point.hpp>
#include <core/helper/utils.hpp>

#include "helper/windows.hpp"
#include "manager/font.hpp"
#include "rect.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <fmt/format.h>
#include <string>

enum class RenderType : u8 { Solid, Blended, Shaded };

struct Texture {
private:
    SDL_Texture* m_raw_texture;

    explicit Texture(SDL_Texture* raw_texture);

public:
    OOPETRIS_GRAPHICS_EXPORTED static Texture
    from_image(SDL_Renderer* renderer, const std::filesystem::path& image_path);

    OOPETRIS_GRAPHICS_EXPORTED static Texture prerender_text(
            SDL_Renderer* renderer,
            const std::string& text,
            const Font& font,
            const Color& color,
            RenderType render_type,
            const Color& background_color
    );

    OOPETRIS_GRAPHICS_EXPORTED static Texture get_for_render_target(SDL_Renderer* renderer, const shapes::UPoint& size);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    OOPETRIS_GRAPHICS_EXPORTED Texture(Texture&& old) noexcept;

    OOPETRIS_GRAPHICS_EXPORTED Texture& operator=(Texture&& other) noexcept;

    OOPETRIS_GRAPHICS_EXPORTED ~Texture();

    template<typename T>
    void render(SDL_Renderer* renderer, const shapes::AbstractRect<T>& rect) const {
        const SDL_Rect rect_sdl = rect.to_sdl_rect();
        SDL_RenderCopy(renderer, m_raw_texture, nullptr, &rect_sdl);
    }

    template<typename T>
    void render(SDL_Renderer* renderer, const shapes::AbstractRect<T>& from, const shapes::URect& dest) const {
        const SDL_Rect from_rect_sdl = from.to_sdl_rect();
        const SDL_Rect to_rect_sdl = dest.to_sdl_rect();
        SDL_RenderCopy(renderer, m_raw_texture, &from_rect_sdl, &to_rect_sdl);
    }

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] shapes::UPoint size() const;

    OOPETRIS_GRAPHICS_EXPORTED void set_as_render_target(SDL_Renderer* renderer) const;
};
