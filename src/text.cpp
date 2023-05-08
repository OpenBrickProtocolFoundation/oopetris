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

void Text::set_position(Point position) {
    m_position = position;
}
