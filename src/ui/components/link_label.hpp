#pragma once

#include "graphics/text.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"

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
                const shapes::URect& fill_rect,
                const Layout& layout,
                bool is_top_level
        );


    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit LinkLabel(
                ServiceProvider* service_provider,
                const std::string& text,
                const std::string& url,
                const Font& font,
                const Color& color,
                const Color& hover_color,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED void on_clicked();

        OOPETRIS_GRAPHICS_EXPORTED void set_text(const ServiceProvider& service_provider, const std::string& text);
    };
} // namespace ui
