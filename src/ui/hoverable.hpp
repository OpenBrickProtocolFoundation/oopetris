#pragma once

#include "helper/bool_wrapper.hpp"
#include "helper/types.hpp"
#include "platform/capabilities.hpp"

#include <cassert>

namespace ui {

    enum class ActionType { Hover, Clicked };

    struct Hoverable {

    private:
        bool m_is_hovered{ false };
        shapes::Rect m_fill_rect;


    public:
        explicit Hoverable(const shapes::Rect& fill_rect)
            : m_fill_rect{ fill_rect } {

              };
        Hoverable(const Hoverable&) = delete;
        Hoverable(Hoverable&&) = delete;
        Hoverable& operator=(const Hoverable&) = delete;
        Hoverable& operator=(Hoverable&&) = delete;
        virtual ~Hoverable() = default;

        [[nodiscard]] auto is_hovered() const {
            return m_is_hovered;
        }


        [[nodiscard]] helper::BoolWrapper<ui::ActionType> detect_hover(const SDL_Event& event, const Window* window) {

            if (utils::device_supports_clicks()) {

                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {
                    if (utils::is_event_in(window, event, m_fill_rect)) {

                        on_hover();

                        if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonDown)) {
                            return { true, ActionType::Clicked };
                        }

                        return { true, ActionType::Hover };
                    }

                    on_unhover();
                    return false;
                }
            }

            return false;
        }


        void on_hover() {
            m_is_hovered = true;
        }

        void on_unhover() {
            m_is_hovered = false;
        }
    };


} // namespace ui
