#pragma once

#include "graphics/text.hpp"
#include "ui/widget.hpp"

#include <string>

namespace ui {
    struct Label final : public Widget {
    private:
        Text m_text;

    public:
        OOPETRIS_GRAPHICS_EXPORTED Label(
                ServiceProvider* service_provider,
                const std::string& text,
                const Font& font,
                const Color& color,
                std::pair<double, double> size,
                Alignment alignment,
                Layout layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& /*event*/) override;

        OOPETRIS_GRAPHICS_EXPORTED void set_text(const ServiceProvider& service_provider, const std::string& text);
    };
} // namespace ui
