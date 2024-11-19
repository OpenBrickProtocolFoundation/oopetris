#include "text.hpp"
#include "graphics/renderer.hpp"
#include "manager/service_provider.hpp"


#if defined(OOPETRIS_DONT_USE_PRERENDERED_TEXT)

Text::Text(
        const ServiceProvider* service_provider,
        const std::string& text,
        const Font& font,
        const Color& color,
        const shapes::URect& dest
)
    : m_font{ font },
      m_color{ color },
      m_dest{ dest },
      m_text{ text } {
    UNUSED(service_provider);
}


void Text::render(const ServiceProvider& service_provider) const {
    auto texture = service_provider.renderer().prerender_text(m_text, m_font, m_color);
    service_provider.renderer().draw_texture(texture, m_dest);
}

void Text::set_text(const ServiceProvider& service_provider, const std::string& text) {
    UNUSED(service_provider);
    m_text = text;
}

#else
Text::Text(
        const ServiceProvider* service_provider,
        const std::string& text,
        const Font& font,
        const Color& color,
        const shapes::URect& dest
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

#endif
