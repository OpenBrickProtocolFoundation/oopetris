#pragma once

#include "color.hpp"
#include "font.hpp"
#include "point.hpp"
#include "rect.hpp"
#include <memory>

struct Application;

struct Text final {
private:
    Point m_position;
    Color m_color;
    std::string m_text;
    Font m_font;

public:
    Text() = default;
    Text(Point position, Color color, std::string text, Font font);
    void render(const Application& app) const;
    void set_text(std::string text);
    void set_position(Point position);
};
