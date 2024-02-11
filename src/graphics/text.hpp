#pragma once

#include "graphics/point.hpp"
#include "graphics/rect.hpp"
#include "helper/color.hpp"
#include "manager/font.hpp"
#include "manager/service_provider.hpp"
#include <memory>

struct Text final {
private:
    Point m_position;
    Color m_color;
    std::string m_text;
    Font m_font;

public:
    Text() = default;
    Text(Point position, Color color, std::string text, Font font);
    void render(const ServiceProvider& service_provider) const;
    void set_text(std::string text);
};


struct ScaledText final {
private:
    Rect m_dest;
    Color m_color;
    std::string m_text;
    Font m_font;

public:
    ScaledText() = default;
    ScaledText(Rect dest, Color color, std::string text, Font font);
    void render(const ServiceProvider& service_provider) const;
    void set_text(std::string text);
};
