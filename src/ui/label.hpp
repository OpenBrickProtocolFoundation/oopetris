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

    public:
        Label(std::string text, Color color, Font font, Layout layout)
            : Widget{ layout },
              m_text{ std::move(text) },
              m_color{ color },
              m_font{ std::move(font) } { }

        void render(const Application& app, const Rect rect) const override {
            const auto absolute_layout = std::get<AbsoluteLayout>(layout);
            const auto position =
                    rect.top_left + Point{ static_cast<int>(absolute_layout.x), static_cast<int>(absolute_layout.y) };

            const auto text = Text{ position, m_color, m_text, m_font };
            text.render(app);
        }

        bool handle_event([[maybe_unused]] const SDL_Event& event) override {
            return false;
        }

        void on_resize_window([[maybe_unused]] const Application& app) override { }
    };
} // namespace ui
