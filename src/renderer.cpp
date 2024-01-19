#include "renderer.hpp"

#include <spdlog/spdlog.h>

namespace {
    constexpr SDL_Rect to_sdl_rect(Rect rect) {
        return SDL_Rect{ rect.top_left.x, rect.top_left.y,
                         static_cast<SDL_Rect_uint>(rect.bottom_right.x - rect.top_left.x + 1),
                         static_cast<SDL_Rect_uint>(rect.bottom_right.y - rect.top_left.y + 1) };
    }
} // namespace

#ifdef _USE_SDL_LEGACY_VERSION


Renderer::Renderer(Window* window) : m_window{ window } { }
#else


Renderer::Renderer(Window& window, const VSync v_sync)
    : m_renderer{
          SDL_CreateRenderer(window.get_sdl_window(), -1, v_sync == VSync::Enabled ? SDL_RENDERER_PRESENTVSYNC : 0)
      } {
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}
#endif

Renderer::~Renderer() {
#ifndef _USE_SDL_LEGACY_VERSION
    SDL_DestroyRenderer(m_renderer);
#endif
}

#ifndef _USE_SDL_LEGACY_VERSION

void Renderer::set_draw_color(Color color) const {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}
#endif

void Renderer::clear(Color clear_color) const {
#ifdef _USE_SDL_LEGACY_VERSION
    SDL_Surface* screen = m_window->get_sdl_screen();

    SDL_FillRect(screen, nullptr, clear_color.to_sdl(screen));
#else
    set_draw_color(clear_color);
    SDL_RenderClear(m_renderer);

#endif
}

void Renderer::draw_rect_filled(Rect rect, Color color) const {
#ifdef _USE_SDL_LEGACY_VERSION
    SDL_Surface* screen = m_window->get_sdl_screen();
    SDL_Rect sdl_rect = to_sdl_rect(rect);
    SDL_FillRect(screen, &sdl_rect, color.to_sdl(screen));
#else
    set_draw_color(color);
    const SDL_Rect sdl_rect = to_sdl_rect(rect);
    SDL_RenderFillRect(m_renderer, &sdl_rect);
#endif
}

void Renderer::draw_rect_outline(Rect rect, Color color) const {
#ifdef _USE_SDL_LEGACY_VERSION

#ifndef _HAVE_SDL_GFX
#error "NEED SDL GFX"
#else
    SDL_Surface* screen = m_window->get_sdl_screen();
    //TODO: is this correct, or is there an off by one error?
    rectangleColor(
            screen, rect.top_left.x, rect.top_left.y, rect.bottom_right.x, rect.bottom_right.y, color.to_sdl(screen)
    );


#endif

#else
    set_draw_color(color);
    const SDL_Rect sdl_rect = to_sdl_rect(rect);
    SDL_RenderDrawRect(m_renderer, &sdl_rect);
#endif
}

void Renderer::present() const {
#ifdef _USE_SDL_LEGACY_VERSION
    SDL_UpdateRect(m_window->get_sdl_screen(), 0, 0, 0, 0);
    SDL_Flip(m_window->get_sdl_screen());
#else
    SDL_RenderPresent(m_renderer);
#endif
}

void Renderer::draw_line(const Point start, const Point end, const Color color) const {
#ifdef _USE_SDL_LEGACY_VERSION

#ifndef _HAVE_SDL_GFX
#error "NEED SDL GFX"
#else
    SDL_Surface* screen = m_window->get_sdl_screen();
    lineColor(screen, start.x, start.y, end.x, end.y, color.to_sdl(screen));


#endif

#else
    set_draw_color(color);
    SDL_RenderDrawLine(m_renderer, start.x, start.y, end.x, end.y);
#endif
}

void Renderer::draw_text(const Point position, const std::string& text, const Font& font, const Color color) const {
#ifdef _USE_SDL_LEGACY_VERSION
    const SDL_Color text_color{ color.r, color.g, color.b, color.a };
    SDL_Surface* const textSurface = TTF_RenderText_Solid(font.m_font.get(), text.c_str(), text_color);
    if (textSurface == nullptr) {
        throw std::runtime_error{ "TTF_RenderText_Solid error: " + std::string{ SDL_GetError() } };
    }
    const auto text_width = textSurface->w;
    const auto text_height = textSurface->h;

    const Rect target_rect{
        position, position + Point{static_cast<SDL_int>(text_width - 1), static_cast<SDL_int>(text_height - 1)}
    };


    SDL_Rect rect{ target_rect.top_left.x, target_rect.top_left.y, 0, 0 };

    int result = SDL_BlitSurface(textSurface, nullptr, m_window->get_sdl_screen(), &rect);
    if (result != 0) {
        throw std::runtime_error{ "SDL_BlitSurface error: " + std::string{ SDL_GetError() } };
    }

    SDL_FreeSurface(textSurface);

#else
    const SDL_Color text_color{ color.r, color.g, color.b, color.a };
    SDL_Surface* const surface = TTF_RenderText_Solid(font.m_font.get(), text.c_str(), text_color);
    SDL_Texture* const texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    const auto text_width = surface->w;
    const auto text_height = surface->h;
    SDL_FreeSurface(surface);
    const Rect target_rect{
        position, position + Point{text_width - 1, text_height - 1}
    };
    const SDL_Rect rect{ target_rect.top_left.x, target_rect.top_left.y,
                         target_rect.bottom_right.x - target_rect.top_left.x + 1,
                         target_rect.bottom_right.y - target_rect.top_left.y + 1 };
    SDL_RenderCopy(m_renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
#endif
}
