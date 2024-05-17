#pragma once

#include "graphics/rect.hpp"
#include "helper/bool_wrapper.hpp"
#include "helper/types.hpp"
#include "helper/utils.hpp"
#include "input/input.hpp"

namespace ui {

    enum class ActionType : u8 { Hover, Clicked, Released };

    struct Hoverable {

    private:
        bool m_is_hovered{ false };
        shapes::URect m_fill_rect;


    public:
        explicit Hoverable(const shapes::URect& fill_rect)
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
        [[nodiscard]] const shapes::URect& fill_rect() const {
            return m_fill_rect;
        }


        [[nodiscard]] helper::BoolWrapper<ui::ActionType>
        detect_hover(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {


            if (const auto result = input_manager->get_pointer_event(event); result.has_value()) {
                if (result->is_in(m_fill_rect)) {

                    on_hover();

                    switch (result->event()) {
                        case input::PointerEvent::PointerDown:
                            return { true, ActionType::Clicked };
                        case input::PointerEvent::PointerUp:
                            return { true, ActionType::Released };

                        case input::PointerEvent::Motion:
                            return { true, ActionType::Hover };

                        default:
                            utils::unreachable();
                    }
                }

                on_unhover();
                return false;
            }


            return false;
        }


        void on_hover() {
            m_is_hovered = true;
        }

        //TODO(Totto): this has to be used correctly, a click or focus change isn't an event, where an unhover needs to happen!
        void on_unhover() {
            m_is_hovered = false;
        }
    };


} // namespace ui
