#pragma once

#include "helper/bool_wrapper.hpp"

#include "manager/service_provider.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layout.hpp"

#include <SDL.h>
#include <utility>

namespace ui {

    enum class EventHandleType : u8 { RequestFocus, RequestUnFocus, RequestAction };

    enum class WidgetType : u8 { Component, Container };

    struct Widget {
    private:
        Layout m_layout;
        WidgetType m_type;
        bool m_top_level;

    public:
        using InnerState = std::pair<ui::EventHandleType, Widget*>;
        using EventHandleResult = helper::BoolWrapper<InnerState>;

        explicit Widget(const Layout& layout, WidgetType type, bool is_top_level)
            : m_layout{ layout },
              m_type{ type },
              m_top_level{ is_top_level } { }

        Widget(const Widget&) = delete;
        Widget(Widget&&) = default;
        Widget& operator=(const Widget&) = delete;
        Widget& operator=(Widget&&) = default;

        virtual ~Widget() = default;

        [[nodiscard]] const Layout& layout() const {
            return m_layout;
        }

        [[nodiscard]] WidgetType type() const {
            return m_type;
        }

        [[nodiscard]] bool is_top_level() const {
            return m_top_level;
        };


        virtual void update() {
            // do nothing
        }
        virtual void render(const ServiceProvider& service_provider) const = 0;
        [[nodiscard]] virtual EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) = 0;
    };

    [[nodiscard]] std::optional<Focusable*> as_focusable(Widget* widget);

    [[nodiscard]] std::optional<Hoverable*> as_hoverable(Widget* widget);


} // namespace ui
