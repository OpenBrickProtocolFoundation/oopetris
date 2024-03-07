#pragma once

#include "graphics/text.hpp"
#include "ui/widget.hpp"
#include <string>

namespace ui {
    struct Label final : public Widget {
    private:
        Text m_text;

    public:
        Label(ServiceProvider* service_provider,
              const std::string& text,
              const Font& font,
              const Color& color,
              std::pair<double, double> size,
              Alignment alignment,
              Layout layout,
              bool is_top_level)
            : Widget{ layout, WidgetType::Component, is_top_level },
              m_text{ service_provider, text, font, color,
                      ui::get_rectangle_aligned(
                              layout,
                              { static_cast<u32>(size.first * layout.get_rect().width()),
                                static_cast<u32>(size.second * layout.get_rect().height()) },
                              alignment
                      ) } { }

        void render(const ServiceProvider& service_provider) const override {
            m_text.render(service_provider);
        }

        helper::BoolWrapper<ui::EventHandleType> handle_event(const SDL_Event&, const Window*) override {
            return false;
        }

        void set_text(const ServiceProvider& service_provider, const std::string& text) {
            m_text.set_text(service_provider, text);
        }
    };
} // namespace ui
