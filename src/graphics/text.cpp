#include "text.hpp"
#include "graphics/renderer.hpp"
#include "manager/service_provider.hpp"


Text::Text(
        ServiceProvider* service_provider,
        const std::string& text,
        const Font& font,
        const Color& color,
        const Rect& dest
)
    : m_font{ font },
      m_color{ color },
      m_dest{ dest },
      m_text{ service_provider->renderer().prerender_text(text, font, color) } { }


void Text::render(const ServiceProvider& service_provider) const {
    service_provider.renderer().draw_texture(m_text, m_dest);
}

void Text::set_text(const ServiceProvider& service_provider, const std::string& text) {
    m_text = service_provider.renderer().prerender_text(text, m_font, m_color);
}
