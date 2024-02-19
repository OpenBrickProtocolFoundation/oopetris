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
        Text m_text;
        Text m_hover_text;
        std::string m_url;

        explicit LinkLabel(
                ServiceProvider* service_provider,
                const std::string& text, // NOLINT(bugprone-easily-swappable-parameters)
                std::string url,
                const Font& font,
                const Color& color, // NOLINT(bugprone-easily-swappable-parameters)
                const Color& hover_color,
                const shapes::Rect& fill_rect,
                const Layout& layout
        )
            : Widget{ layout },
              Hoverable{ fill_rect },
              m_text{ service_provider, text, font, color, fill_rect },
              m_hover_text{ service_provider, text, font, hover_color, fill_rect },
              m_url{ std::move(url) } { }


    public:
        explicit LinkLabel(
                ServiceProvider* service_provider,
                const std::string& text,
                const std::string& url,
                const Font& font,
                const Color& color,
                const Color& hover_color,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout
        )
            : LinkLabel{ service_provider,
                         text,
                         url,
                         font,
                         color,
                         hover_color,
                         ui::get_rectangle_aligned(
                                 layout,
                                 { static_cast<u32>(size.first * layout.get_rect().width()),
                                   static_cast<u32>(size.second * layout.get_rect().height()) },
                                 alignment
                         ),
                         layout } { }

        void render(const ServiceProvider& service_provider) const override {
            if (is_hovered()) {
                m_hover_text.render(service_provider);
            } else {
                m_text.render(service_provider);
            }
        }

        bool handle_event(const SDL_Event& event, const Window* window) override {
            return detect_hover(event, window);
        }

        void on_clicked() override {
            const auto result = utils::open_url(m_url);
            if (not result) {
                spdlog::error("Couldn't open link label");
            }
        }

        void set_text(const ServiceProvider& service_provider, const std::string& text) {
            m_text.set_text(service_provider, text);
            m_hover_text.set_text(service_provider, text);
        }
    };
} // namespace ui
