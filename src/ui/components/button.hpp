#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <utility>

#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "manager/resource_manager.hpp"
#include "platform/capabilities.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"


namespace ui {
    struct Button : public Widget, public Focusable, public Hoverable {
    public:
        using Callback = std::function<void(const Button&)>;

    private:
        Text m_text;
        Callback m_callback;
        shapes::URect m_fill_rect;
        bool m_enabled;

        explicit Button(
                ServiceProvider* service_provider,
                const std::string& text,
                usize focus_id,
                Callback callback,
                const Font& font,
                const Color& text_color,
                const shapes::URect& fill_rect,
                std::pair<u32, u32> margin,
                const Layout& layout,
                bool is_top_level
        );

    public:
        explicit Button(
                ServiceProvider* service_provider,
                const std::string& text,
                const Font& font,
                const Color& text_color,
                usize focus_id,
                Callback callback,
                std::pair<double, double> size,
                Alignment alignment,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level
        );


        void render(const ServiceProvider& service_provider) const override;

        helper::BoolWrapper<ui::EventHandleType> handle_event(const SDL_Event& event, const Window* window) override;

        void on_clicked();

        void disable();

        void enable();

        [[nodiscard]] bool is_enabled() const;
    };

} // namespace ui
