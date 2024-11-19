#pragma once

#include <core/helper/color.hpp>

#include "helper/windows.hpp"
#include "manager/font.hpp"
#include "manager/service_provider.hpp"
#include "rect.hpp"
#include "texture.hpp"

//TODO(Totto): set this flag in the build system, or maybe also fix https://github.com/OpenBrickProtocolFoundation/oopetris/issues/132 in the process
#if defined(__EMSCRIPTEN__)
#define OOPETRIS_DONT_USE_PRERENDERED_TEXT
#endif


struct Text final {
private:
    Font m_font;
    Color m_color;
    shapes::URect m_dest;
#if defined(OOPETRIS_DONT_USE_PRERENDERED_TEXT)
    std::string m_text;
#else
    Texture m_text;
#endif
public:
    OOPETRIS_GRAPHICS_EXPORTED Text(
            const ServiceProvider* service_provider,
            const std::string& text,
            const Font& font,
            const Color& color,
            const shapes::URect& dest
    );

    OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const;
    OOPETRIS_GRAPHICS_EXPORTED void set_text(const ServiceProvider& service_provider, const std::string& text);
};
