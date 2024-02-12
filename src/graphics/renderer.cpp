#include "graphics/renderer.hpp"

namespace {
    constexpr SDL_Rect to_sdl_rect(Rect rect) {
        return SDL_Rect{ rect.top_left.x, rect.top_left.y, rect.bottom_right.x - rect.top_left.x + 1,
                         rect.bottom_right.y - rect.top_left.y + 1 };
    }
} // namespace

Renderer::Renderer(Window& window, const VSync v_sync)
    : m_renderer{
          SDL_CreateRenderer(window.get_sdl_window(), -1, v_sync == VSync::Enabled ? SDL_RENDERER_PRESENTVSYNC : 0)
      } {
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(m_renderer);
}

void Renderer::set_draw_color(Color color) const {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void Renderer::clear(Color clear_color) const {
    set_draw_color(clear_color);
    SDL_RenderClear(m_renderer);
}

void Renderer::draw_rect_filled(Rect rect, Color color) const {
    set_draw_color(color);
    const SDL_Rect sdl_rect = to_sdl_rect(rect);
    SDL_RenderFillRect(m_renderer, &sdl_rect);
}

void Renderer::draw_rect_outline(Rect rect, Color color) const {
    set_draw_color(color);
    const SDL_Rect sdl_rect = to_sdl_rect(rect);
    SDL_RenderDrawRect(m_renderer, &sdl_rect);
}

void Renderer::present() const {
    SDL_RenderPresent(m_renderer);
}

void Renderer::draw_line(const Point start, const Point end, const Color color) const {
    set_draw_color(color);
    SDL_RenderDrawLine(m_renderer, start.x, start.y, end.x, end.y);
}

void Renderer::draw_text(const Point position, const std::string& text, const Font& font, const Color color) const {
    const SDL_Color text_color{ color.r, color.g, color.b, color.a };
    SDL_Surface* const surface = TTF_RenderText_Solid(font.m_font.get(), text.c_str(), text_color);
    SDL_Texture* const texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    const auto text_width = surface->w;
    const auto text_height = surface->h;
    SDL_FreeSurface(surface);
    const Rect target_rect{
        position, position + Point{text_width - 1, text_height - 1}
    };
    const SDL_Rect rect = target_rect.to_sdl_rect();
    SDL_RenderCopy(m_renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
}


void Renderer::draw_text(const Rect dest, const std::string& text, const Font& font, const Color color) const {
    const SDL_Color text_color{ color.r, color.g, color.b, color.a };
    SDL_Surface* const surface = TTF_RenderText_Solid(font.m_font.get(), text.c_str(), text_color);
    SDL_Texture* const texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    const SDL_Rect rect = dest.to_sdl_rect();
    SDL_RenderCopy(m_renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
}


void Renderer::draw_texture(const Texture& texture, const Rect& rect) const {
    texture.render(m_renderer, rect);
}

Texture Renderer::load_image(const std::string& image_path) const {
    return Texture::from_image(m_renderer, image_path);
}
