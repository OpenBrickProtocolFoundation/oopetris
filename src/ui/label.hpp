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
        std::pair<u32, u32> m_size;
        Alignment m_alignment;

        [[nodiscard]] inline Rect get_fill_rect() const {
            return ui::get_rectangle_aligned(layout, m_size.first, m_size.second, m_alignment);
        }

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
              m_size{ static_cast<u32>(size.first * layout.get_rect().width()),
                      static_cast<u32>(size.second * layout.get_rect().height()) },
              m_alignment{ alignment } { }

        void render(const ServiceProvider& service_provider) const override {
            const auto rect = get_fill_rect();

            const auto text = ScaledText{ rect, m_color, m_text, m_font };
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
