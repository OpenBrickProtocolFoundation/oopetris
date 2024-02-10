#include "text.hpp"
#include "renderer.hpp"

Text::Text(Point position, Color color, std::string text, Font font)
    : m_position{ position },
      m_color{ color },
      m_text{ std::move(text) },
      m_font{ std::move(font) } { }


void Text::render(const ServiceProvider& service_provider) const {
    service_provider.renderer().draw_text(m_position, m_text, m_font, m_color);
}

void Text::set_text(std::string text) {
    m_text = std::move(text);
}


ScaledText::ScaledText(Rect dest, Color color, std::string text, Font font)
    : m_dest{ dest },
      m_color{ color },
      m_text{ std::move(text) },
      m_font{ std::move(font) } { }


void ScaledText::render(const ServiceProvider& service_provider) const {
    service_provider.renderer().draw_text(m_dest, m_text, m_font, m_color);
}

void ScaledText::set_text(std::string text) {
    m_text = std::move(text);
}
