
#include "tile_layout.hpp"


void ui::TileLayout::render(const ServiceProvider& service_provider) const {
    for (const auto& widget : m_widgets) {
        widget->render(service_provider);
    }
}

helper::BoolWrapper<ui::EventHandleType> ui::TileLayout::handle_event(
        const SDL_Event& event,
        const Window* window
) // NOLINT(readability-function-cognitive-complexity)
{
    helper::BoolWrapper<ui::EventHandleType> handled = handle_focus_change_events(event, window);

    if (handled) {
        return handled;
    }

    if (utils::device_supports_clicks()) {

        if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {

            for (auto& widget : m_widgets) {
                const auto layout = widget->layout();
                if (not handled and utils::is_event_in(window, event, layout.get_rect())) {
                    if (const auto event_result = widget->handle_event(event, window); event_result) {
                        handled = { true, handle_event_result(event_result.get_additional(), widget.get()) };
                        continue;
                    }
                } else {
                    const auto hoverable = ui::as_hoverable(widget.get());
                    if (hoverable.has_value()) {
                        hoverable.value()->on_unhover();
                    }
                }
            }

            return handled;
        }
    }

    return handled;
}


[[nodiscard]] ui::Layout ui::TileLayout::get_layout_for_index(u32 index) {
    if (index >= this->size) {
        throw std::runtime_error("TileLayout is already full");
    }

    const auto start_point = layout().get_rect().top_left;

    u32 x = start_point.x + margin.first;
    u32 y = start_point.y + margin.second;
    u32 width = layout().get_rect().width() - (margin.first * 2);
    u32 height = layout().get_rect().height() - (margin.second * 2);

    if (direction == Direction::Horizontal) {
        const auto previous_start =
                index == 0 ? 0 : static_cast<u32>(width * steps.at(index - 1)) + gap.get_margin() / 2;

        const auto current_end =
                index == this->size - 1
                        ? width
                        : (steps.size() <= index ? width
                                                 : static_cast<u32>(width * steps.at(index)) - gap.get_margin() / 2);

        width = current_end - previous_start;
        x += previous_start;
    } else {
        const auto previous_start =
                index == 0 ? 0 : static_cast<u32>(height * steps.at(index - 1)) + gap.get_margin() / 2;

        const auto current_end =
                index == this->size - 1
                        ? height
                        : (steps.size() <= index ? height
                                                 : static_cast<u32>(height * steps.at(index)) - gap.get_margin() / 2);

        height = current_end - previous_start;
        y += previous_start;
    }


    return AbsolutLayout{
        x,
        y,
        width,
        height,
    };
}
