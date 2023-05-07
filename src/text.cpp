#include "text.hpp"
#include "renderer.hpp"

Text::Text(Point position, Color color, std::string text, Font font)
    : m_position{ position },
      m_color{ color },
      m_text{ std::move(text) },
      m_font{ std::move(font) } { }


void Text::render(const ServiceProvider& service_provider) const {
    const SDL_Color textColor{ m_color.r, m_color.g, m_color.b, m_color.a };
    SDL_Surface* surface = TTF_RenderText_Solid(m_font.m_font.get(), m_text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(service_provider.renderer().m_renderer, surface);
    const int text_width = surface->w;
    const int text_height = surface->h;
    SDL_FreeSurface(surface);
    const Rect target_rect{
        m_position, m_position + Point{text_width - 1, text_height - 1}
    };
    const SDL_Rect rect{ target_rect.top_left.x, target_rect.top_left.y,
                         target_rect.bottom_right.x - target_rect.top_left.x + 1,
                         target_rect.bottom_right.y - target_rect.top_left.y + 1 };
    SDL_RenderCopy(service_provider.renderer().m_renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
}

void Text::set_text(std::string text) {
    m_text = std::move(text);
}

void Text::set_position(Point position) {
    m_position = position;
}
