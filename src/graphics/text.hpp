#pragma once

#include <core/helper/color.hpp>

#include "manager/font.hpp"
#include "manager/service_provider.hpp"
#include "rect.hpp"
#include "texture.hpp"

struct Text final {
private:
    Font m_font;
    Color m_color;
    shapes::URect m_dest;
    Texture m_text;

public:
    Text(const ServiceProvider* service_provider,
         const std::string& text,
         const Font& font,
         const Color& color,
         const shapes::URect& dest);

    void render(const ServiceProvider& service_provider) const;
    void set_text(const ServiceProvider& service_provider, const std::string& text);
};
