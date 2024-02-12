#pragma once
#include "helper/types.hpp"
#include <cassert>

namespace ui {

    struct Hoverable {

    private:
        bool m_is_hovered{ false };
        Rect m_fill_rect;


    public:
        explicit Hoverable(const Rect& fill_rect)
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


        [[nodiscard]] bool detect_hover(const SDL_Event& event, const Window* window) {

            if (utils::device_supports_clicks()) {

                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {
                    //TODO: this hover logic is bad at detecting un-hovers, especially on android, fix that
                    if (utils::is_event_in(window, event, m_fill_rect)) {

                        on_hover();

                        if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonDown)) {
                            on_clicked();
                        }

                        return true;
                    }

                    on_unhover();
                    return false;
                }
            }

            return false;
        }

        virtual void on_clicked() = 0;


    protected:
        void on_hover() {
            m_is_hovered = true;
        }

        void on_unhover() {
            m_is_hovered = false;
        }
    };

} // namespace ui
