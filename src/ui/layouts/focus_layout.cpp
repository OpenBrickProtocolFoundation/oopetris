
#include "focus_layout.hpp"
#include "helper/optional.hpp"
#include "input/input.hpp"
#include "ui/widget.hpp"

#include <ranges>


ui::FocusLayout::FocusLayout(const Layout& layout, u32 focus_id, FocusOptions options, bool is_top_level)
    : Widget{ layout, WidgetType::Container, is_top_level },
      Focusable{ focus_id },
      Hoverable{ layout.get_rect() },

      m_options{ options } {

    // if on top. we give us focus automatically
    if (is_top_level) {
        focus();
    }
}


void ui::FocusLayout::update() {
    for (auto& widget : m_widgets) {
        widget->update();
    }
}

[[nodiscard]] u32 ui::FocusLayout::widget_count() const {
    return static_cast<u32>(m_widgets.size());
}


ui::Widget::EventHandleResult
ui::FocusLayout::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {
    Widget::EventHandleResult handled = handle_focus_change_events(input_manager, event);

    if (handled) {
        return handled;
    }


    if (const auto pointer_event = input_manager->get_pointer_event(event); pointer_event.has_value()) {

        for (auto& widget : m_widgets) {
            const auto layout = widget->layout();
            if (not handled and pointer_event->is_in(layout.get_rect())) {
                if (const auto event_result = widget->handle_event(input_manager, event); event_result) {
                    handled = { true, handle_event_result(event_result.get_additional(), widget.get()) };
                    continue;
                }
            } else {
                const auto hoverable = as_hoverable(widget.get());
                if (hoverable.has_value()) {
                    hoverable.value()->on_unhover();
                }
            }
        }
        return handled;
    }


    return handled;
}


ui::Widget::EventHandleResult ui::FocusLayout::handle_focus_change_button_events(
        const std::shared_ptr<input::InputManager>& input_manager,
        const SDL_Event& event
) {

    Widget::EventHandleResult handled = false;

    const auto navigation_action = input_manager->get_navigation_event(event);

    if (navigation_action == input::NavigationEvent::DOWN
        or (m_options.allow_tab and navigation_action == input::NavigationEvent::TAB)) {
        handled = try_set_next_focus(FocusChangeDirection::Forward);
    } else if (navigation_action == input::NavigationEvent::UP) {
        handled = try_set_next_focus(FocusChangeDirection::Backward);
    }


    return handled;
}


ui::Widget::EventHandleResult ui::FocusLayout::handle_focus_change_events(
        const std::shared_ptr<input::InputManager>& input_manager,
        const SDL_Event& event
) {


    if (not has_focus()) {
        return false;
    }

    Widget::EventHandleResult handled = false;


    if (m_focus_id.has_value()) {
        const auto& widget = m_widgets.at(focusable_index_by_id(m_focus_id.value()));


        if (widget->type() != WidgetType::Container) {
            handled = handle_focus_change_button_events(input_manager, event);
        }


        if (handled) {
            return handled;
        }

        if (const auto event_result = widget->handle_event(input_manager, event); event_result) {

            return { true, handle_event_result(event_result.get_additional(), widget.get()) };
        }


        if (widget->type() == WidgetType::Container) {
            handled = handle_focus_change_button_events(input_manager, event);
        }
    }


    return handled;
}

[[nodiscard]] helper::optional<ui::Widget::InnerState>
ui::FocusLayout::handle_event_result(const helper::optional<ui::Widget::InnerState>& result, Widget* widget) {

    if (not result.has_value()) {
        return helper::nullopt;
    }

    auto value = result.value();

    switch (value.first) {
        case ui::EventHandleType::RequestFocus: {
            const auto focusable = as_focusable(widget);
            if (not focusable.has_value()) {
                throw std::runtime_error("Only Focusables can request focus!");
            }

            if (not m_focus_id.has_value()) {
                return helper::nullopt;
            }

            const auto widget_focus_id = focusable.value()->focus_id();

            if (m_focus_id.value() == widget_focus_id) {
                return helper::nullopt;
            }

            auto current_focusable = as_focusable(m_widgets.at(focusable_index_by_id(m_focus_id.value())).get());
            assert(current_focusable.has_value());
            current_focusable.value()->unfocus(); // NOLINT(bugprone-unchecked-optional-access)
            focusable.value()->focus();           // NOLINT(bugprone-unchecked-optional-access)
            m_focus_id = widget_focus_id;

            // if the layout itself has not focus, it needs focus itself too
            if (not has_focus()) {
                return ui::Widget::InnerState{ ui::EventHandleType::RequestFocus, value.second };
            }


            return helper::nullopt;
        }
        case ui::EventHandleType::RequestUnFocus: {
            if (not has_focus()) {
                return helper::nullopt;
            }

            const auto focusable = as_focusable(widget);
            if (not focusable.has_value()) {
                throw std::runtime_error("Only Focusables can request un-focus!");
            }


            if (not m_focus_id.has_value()) {
                return helper::nullopt;
            }

            const auto widget_focus_id = focusable.value()->focus_id();

            if (m_focus_id.value() != widget_focus_id) {
                return helper::nullopt;
            }

            // try to unfocus, in forward direction, if that fails request unfocus of they container / layout itself, also test backwards, if not wrapping!
            const auto test_forward = try_set_next_focus(FocusChangeDirection::Forward);
            if (not test_forward) {
                if (m_options.wrap_around) {
                    return ui::Widget::InnerState{ ui::EventHandleType::RequestUnFocus, value.second };
                }

                const auto test_backwards = try_set_next_focus(FocusChangeDirection::Backward);
                if (not test_backwards) {
                    return ui::Widget::InnerState{ ui::EventHandleType::RequestUnFocus, value.second };
                }
            }

            return helper::nullopt;
        }
        case ui::EventHandleType::RequestAction: {
            // just forward it
            return ui::Widget::InnerState{ ui::EventHandleType::RequestAction, value.second };
        }
        default:
            utils::unreachable();
    }
}

[[nodiscard]] u32 ui::FocusLayout::focusable_index_by_id(const u32 id) const {
    const auto find_iterator = std::ranges::find_if(m_widgets, [id](const std::unique_ptr<Widget>& widget) {
        const auto focusable = as_focusable(widget.get());
        return focusable.has_value() and focusable.value()->focus_id() == id;
    });
    assert(find_iterator != m_widgets.end());
    const auto index = static_cast<u32>(std::distance(m_widgets.begin(), find_iterator));
    return index;
}

[[nodiscard]] std::vector<u32> ui::FocusLayout::focusable_ids_sorted() const {
    auto result = std::vector<u32>{};
    for (const auto& widget : m_widgets) {
        const auto focusable = as_focusable(widget.get());
        if (focusable) {
            result.push_back((*focusable)->focus_id());
        }
    }

#ifdef DEBUG_BUILD
    // this works, since result is sorted already
    const auto duplicates = std::ranges::adjacent_find(result);
    if (duplicates != result.cend()) {
        throw std::runtime_error("Focusables have duplicates: " + std::to_string(*duplicates));
    }
#endif
    std::ranges::sort(result);
    return result;
}

[[nodiscard]] u32 ui::FocusLayout::index_of(const std::vector<u32>& ids, const u32 needle) {
    return static_cast<u32>(std::distance(ids.cbegin(), std::ranges::find(ids, needle)));
}

[[nodiscard]] bool ui::FocusLayout::try_set_next_focus(const FocusChangeDirection focus_direction) {

    if (not m_focus_id.has_value()) {
        return false;
    }

    const auto focusable_ids = focusable_ids_sorted();

    assert(not focusable_ids.empty() && "if something is focused, we should have at least one focusable widget");

    if (focusable_ids.size() == 1) {
        return false;
    }

    const int current_index = static_cast<int>(index_of(focusable_ids, m_focus_id.value()));
    int next_index = (focus_direction == FocusChangeDirection::Forward ? current_index + 1 : current_index - 1);

    if (next_index < 0) {
        if (not m_options.wrap_around) {
            return false;
        }
        next_index = static_cast<int>(focusable_ids.size()) + next_index;
    }

    if (next_index >= static_cast<int>(focusable_ids.size())) {
        if (not m_options.wrap_around) {
            return false;
        }

        next_index = next_index % static_cast<int>(focusable_ids.size());
    }


    auto current_focusable = as_focusable(m_widgets.at(focusable_index_by_id(focusable_ids.at(current_index))).get());
    assert(current_focusable.has_value());
    auto next_focusable = as_focusable(m_widgets.at(focusable_index_by_id(focusable_ids.at(next_index))).get());
    assert(next_focusable.has_value());
    current_focusable.value()->unfocus();            // NOLINT(bugprone-unchecked-optional-access)
    next_focusable.value()->focus();                 // NOLINT(bugprone-unchecked-optional-access)
    m_focus_id = next_focusable.value()->focus_id(); // NOLINT(bugprone-unchecked-optional-access)

    return true;
}

void ui::FocusLayout::give_focus(Focusable* focusable) {
    m_focus_id = focusable->focus_id();
    focusable->focus();
}
