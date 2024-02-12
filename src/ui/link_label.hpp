#pragma once

#include "graphics/text.hpp"
#include "hoverable.hpp"
#include "platform/capabilities.hpp"
#include "ui/widget.hpp"

#include <spdlog/spdlog.h>
#include <string>

namespace ui {
    struct LinkLabel final : public Widget, public Hoverable {
    private:
        std::string m_text;
        std::string m_url;
        Color m_color;
        Color m_hover_color;
        Font m_font;
        Rect m_fill_rect;

        explicit LinkLabel(
                std::string text,
                std::string url,
                Color color,
                Color hover_color,
                Font font,
                const Rect& fill_rect,
                const Layout& layout
        )
            : Widget{ layout },
              Hoverable{ fill_rect },
              m_text{ std::move(text) },
              m_url{ std::move(url) },
              m_color{ color },
              m_hover_color{ hover_color },
              m_font{ std::move(font) },
              m_fill_rect{ fill_rect } { }


    public:
        explicit LinkLabel(
                std::string text,
                std::string url,
                Color color,
                Color hover_color,
                Font font,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout
        )
            : LinkLabel{ std::move(text),
                         std::move(url),
                         color,
                         hover_color,
                         std::move(font),
                         ui::get_rectangle_aligned(
                                 layout,
                                 static_cast<u32>(size.first * layout.get_rect().width()),
                                 static_cast<u32>(size.second * layout.get_rect().height()),
                                 alignment
                         ),
                         layout } { }

        void render(const ServiceProvider& service_provider) const override {
            const auto color = is_hovered() ? m_hover_color : m_color;

            const auto text = ScaledText{ m_fill_rect, color, m_text, m_font };
            text.render(service_provider);
        }

        bool handle_event(const SDL_Event& event, const Window* window) override {

            if (detect_hover(event, window)) {
                return true;
            }

            return false;
        }

        void on_clicked() override {
            const auto result = utils::open_url(m_url);
            if (not result) {
                spdlog::error("Couldn't open link label");
            }
        }

        void set_text(const std::string& text) {
            m_text = text;
        }
    };
} // namespace ui
