
#include "link_label.hpp"
#include "helper/platform.hpp"


ui::LinkLabel::LinkLabel(
        ServiceProvider* service_provider,
        const std::string& text, // NOLINT(bugprone-easily-swappable-parameters)
        std::string url,
        const Font& font,
        const Color& color, // NOLINT(bugprone-easily-swappable-parameters)
        const Color& hover_color,
        const shapes::URect& fill_rect,
        const Layout& layout,
        bool is_top_level
)
    : Widget{ layout, WidgetType::Component, is_top_level },
      Hoverable{ fill_rect },
      m_text{ service_provider, text, font, color, fill_rect },
      m_hover_text{ service_provider, text, font, hover_color, fill_rect },
      m_url{ std::move(url) } { }


ui::LinkLabel::LinkLabel(
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
                 layout,
                 is_top_level } { }

void ui::LinkLabel::render(const ServiceProvider& service_provider) const {
    if (is_hovered()) {
        m_hover_text.render(service_provider);
    } else {
        m_text.render(service_provider);
    }
}

ui::Widget::EventHandleResult
ui::LinkLabel::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {
    if (const auto hover_result = detect_hover(input_manager, event); hover_result) {
        if (hover_result.is(ActionType::Clicked)) {
            on_clicked();
        }
        return true;
    }

    return false;
}

void ui::LinkLabel::on_clicked() {
    const auto result = utils::open_url(m_url);
    if (not result) {
        spdlog::error("Couldn't open link label");
    }
}

void ui::LinkLabel::set_text(const ServiceProvider& service_provider, const std::string& text) {
    m_text.set_text(service_provider, text);
    m_hover_text.set_text(service_provider, text);
}
