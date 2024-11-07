#include <core/helper/errors.hpp>

#include "renderer.hpp"


//TODO(Totto):  assert return values of all sdl functions


Renderer::Renderer(SDL_Renderer* renderer) : m_renderer{ renderer } {

    if (m_renderer == nullptr) {
        throw helper::InitializationError{ fmt::format("Failed creating a SDL Renderer: {}", SDL_GetError()) };
    }

    auto result = SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    if (result < 0) {
        throw helper::InitializationError{ fmt::format("Failed in setting BlendMode on Renderer: {}", SDL_GetError()) };
    }
}

Renderer::Renderer(const Window& window, const VSync v_sync)
    : Renderer{ SDL_CreateRenderer(
              window.get_sdl_window(),
              -1,
              (v_sync == VSync::Enabled ? SDL_RENDERER_PRESENTVSYNC : 0) | SDL_RENDERER_TARGETTEXTURE
#if defined(__3DS__) || defined(__SERENITY__)
                      | SDL_RENDERER_SOFTWARE
#else
                      | SDL_RENDERER_ACCELERATED
#endif
      ) } {
}

Renderer Renderer::get_software_renderer(std::unique_ptr<SDL_Surface>& surface) {
    return Renderer{ SDL_CreateSoftwareRenderer(surface.get()) };
}

Renderer::Renderer(Renderer&& other) noexcept : m_renderer{ other.m_renderer } {
    other.m_renderer = nullptr;
}

Renderer::~Renderer() {
    if (m_renderer != nullptr) {
        SDL_DestroyRenderer(m_renderer);
    }
}

void Renderer::set_draw_color(const Color& color) const {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void Renderer::clear(const Color& clear_color) const {
    set_draw_color(clear_color);
    const int result = SDL_RenderClear(m_renderer);
    ASSERT(result == 0 && "render clear was executed without error");
}

void Renderer::present() const {
    SDL_RenderPresent(m_renderer);
}

Texture Renderer::load_image(const std::filesystem::path& image_path) const {
    return Texture::from_image(m_renderer, image_path);
}

Texture Renderer::prerender_text(
        const std::string& text,
        const Font& font,
        const Color& color,
        RenderType render_type,
        const Color& background_color
) const {
    return Texture::prerender_text(m_renderer, text, font, color, render_type, background_color);
}

Texture Renderer::get_texture_for_render_target(const shapes::UPoint& size) const {


    const auto supported = SDL_RenderTargetSupported(m_renderer);

    if (supported == SDL_FALSE) {
        throw helper::FatalError{ "SDL does not support a target renderer, but we need one!" };
    }

    return Texture::get_for_render_target(m_renderer, size);
}


void Renderer::set_render_target(const Texture& texture) const {
    texture.set_as_render_target(m_renderer);
}

void Renderer::reset_render_target() const {
    const auto result = SDL_SetRenderTarget(m_renderer, nullptr);
    if (result < 0) {
        throw helper::FatalError{ fmt::format("Failed to set render texture target with error: {}", SDL_GetError()) };
    }
}

void Renderer::draw_self_computed_circle_impl(const shapes::IPoint& center, i32 diameter) const {

    //taken from: https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl

    const auto [center_x, center_y] = center;

    const i32 radius = diameter / 2;

    i32 x_pos = radius - 1;
    i32 y_pos = 0;
    i32 tx_pos = 1;
    i32 ty_pos = 1;
    i32 error = tx_pos - diameter;

    while (x_pos >= y_pos) {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(
                m_renderer, center_x + x_pos, // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
                center_y - y_pos
        );
        SDL_RenderDrawPoint(m_renderer, center_x + x_pos, center_y + y_pos);
        SDL_RenderDrawPoint(m_renderer, center_x - x_pos, center_y - y_pos);
        SDL_RenderDrawPoint(m_renderer, center_x - x_pos, center_y + y_pos);
        SDL_RenderDrawPoint(m_renderer, center_x + y_pos, center_y - x_pos);
        SDL_RenderDrawPoint(m_renderer, center_x + y_pos, center_y + x_pos);
        SDL_RenderDrawPoint(m_renderer, center_x - y_pos, center_y - x_pos);
        SDL_RenderDrawPoint(m_renderer, center_x - y_pos, center_y + x_pos);

        if (error <= 0) {
            ++y_pos;
            error += ty_pos;
            ty_pos += 2;
        }

        if (error > 0) {
            --x_pos;
            tx_pos += 2;
            error += (tx_pos - diameter);
        }
    }
}
