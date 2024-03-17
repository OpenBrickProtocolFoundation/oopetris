
#include "button.hpp"


ui::Button::Button(
                ServiceProvider* service_provider,
                const std::string& text,
                u32 focus_id,
                Callback callback,
                const Font& font,
                const Color& text_color,
                const shapes::URect& fill_rect,
                std::pair<u32, u32> margin,
                const Layout& layout,
                bool is_top_level
        )
            : Widget{ layout, WidgetType::Component, is_top_level },
              Focusable{
                  focus_id
        },
              Hoverable{ fill_rect },
              m_text{ service_provider,
                      text,
                      font,
                      text_color,
                      { fill_rect.top_left.x + static_cast<int>(margin.first),
                        fill_rect.top_left.y + static_cast<int>(margin.second),
                        fill_rect.width() - 2 * static_cast<int>(margin.first),
                        fill_rect.height() - 2 * static_cast<int>(margin.second) } },
              m_callback{ std::move(callback) },
              m_fill_rect{ fill_rect },
              m_enabled{ true } {

    // if on top. we give us focus automatically
    if (is_top_level) {
        focus();
    }
}


ui::Button::Button(
        ServiceProvider* service_provider,
        const std::string& text,
        const Font& font,
        const Color& text_color,
        u32 focus_id,
        Callback callback,
        std::pair<double, double> size,
        Alignment alignment,
        std::pair<double, double> margin,
        const Layout& layout,
        bool is_top_level
)
    : Button{
          service_provider,
          text,
          focus_id,
          std::move(callback),
          font,
          text_color,
          ui::get_rectangle_aligned(
                  layout,
                  {               static_cast<u32>(size.first * layout.get_rect().width()),
                    static_cast<u32>(size.second * layout.get_rect().height())                },
                  alignment
          ),
          {static_cast<u32>(margin.first * size.first * layout.get_rect().width()),
                    static_cast<u32>(margin.second * size.second * layout.get_rect().height())},
          layout,
          is_top_level
} { }


void ui::Button ::render(const ServiceProvider& service_provider) const {
    const auto color = not m_enabled ? (has_focus() ? "#A36A6A"_rgb : "#919191"_rgb)
                                     : (has_focus()    ? is_hovered() ? "#FF6A00"_rgb : Color::red()
                                           : is_hovered() ? "#00BBFF"_rgb
                                                       : Color::blue());
    service_provider.renderer().draw_rect_filled(m_fill_rect, color);

    m_text.render(service_provider);
}

helper::BoolWrapper<ui::EventHandleType> ui::Button::handle_event(const SDL_Event& event, const Window* window) {
    if (not m_enabled) {
        return false;
    }

    if (utils::device_supports_keys()) {
        if (has_focus() and utils::event_is_action(event, utils::CrossPlatformAction::OK)) {
            spdlog::info("button pressed");
            on_clicked();
            return true;
        }
    }

    if (const auto hover_result = detect_hover(event, window); hover_result) {
        if (hover_result.is(ActionType::Clicked)) {
            on_clicked();
        }
        return true;
    }

    return false;
}


void ui::Button::on_clicked() {
    m_callback(*this);
}

void ui::Button::disable() {
    m_enabled = false;
}

void ui::Button::enable() {
    m_enabled = true;
}

[[nodiscard]] bool ui::Button::is_enabled() const {
    return m_enabled;
}
