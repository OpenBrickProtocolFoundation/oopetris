#pragma once

#include "../text.hpp"
#include "widget.hpp"
#include <string>

namespace ui {
    struct Label final : public Widget {
    private:
        std::string m_text;
        Color m_color;
        Font m_font;
        Rect fill_rect;

    public:
        Label(std::string text,
              Color color,
              Font font,
              std::pair<double, double> size,
              Alignment alignment,
              Layout layout)
            : Widget{ layout },
              m_text{ std::move(text) },
              m_color{ color },
              m_font{ std::move(font) },
              fill_rect{ ui::get_rectangle_aligned(
                      layout,
                      static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()),
                      alignment
              ) }

        { }

        void render(const ServiceProvider& service_provider) const override {

            const auto text = ScaledText{ fill_rect, m_color, m_text, m_font };
            text.render(service_provider);
        }

        bool handle_event(const SDL_Event&, const Window*) override {
            return false;
        }

        void set_text(const std::string& text) {
            m_text = text;
        }
    };
} // namespace ui
