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
              bool is_top_level);

        void render(const ServiceProvider& service_provider) const override;

        [[nodiscard]] Widget::EventHandleResult handle_event(const SDL_Event&, const Window*) override;

        void set_text(const ServiceProvider& service_provider, const std::string& text);
    };
} // namespace ui
