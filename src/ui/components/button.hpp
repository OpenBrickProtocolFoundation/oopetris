#pragma once

#include <core/helper/color_literals.hpp>

#include "helper/spdlog_wrapper.hpp"
#include <functional>
#include <utility>

#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "input/input.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"


namespace ui {
    template<typename Content, typename CallBackType>
    struct Button : public Widget, public Focusable, public Hoverable {
    public:
        using Callback = std::function<bool(const CallBackType&)>;

    private:
        Content m_content;
        Callback m_callback;
        shapes::URect m_fill_rect;
        bool m_enabled;

    protected:
        explicit Button(
                Content&& content,
                u32 focus_id,
                Callback callback,
                const shapes::URect& fill_rect,
                const Layout& layout,
                bool is_top_level
        )
            : Widget{ layout, WidgetType::Component, is_top_level },
              Focusable{ focus_id },
              Hoverable{ fill_rect },
              m_content{ std::move(content) },
              m_callback{ std::move(callback) },
              m_fill_rect{ fill_rect },
              m_enabled{ true } {

            // if on top. we give us focus automatically
            if (is_top_level) {
                focus();
            }
        }

        [[nodiscard]] const Callback& callback() const {
            return m_callback;
        };

    public:
        explicit Button(
                Content&& content,
                u32 focus_id,
                Callback callback,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout,
                bool is_top_level
        )
            : Button{ std::move(content),
                      focus_id,
                      std::move(callback),
                      ui::get_rectangle_aligned(
                              layout,
                              { static_cast<u32>(size.first * layout.get_rect().width()),
                                static_cast<u32>(size.second * layout.get_rect().height()) },
                              alignment
                      ),
                      layout,
                      is_top_level } { }


        void render(const ServiceProvider& service_provider) const override {

            //TODO(Totto): get as input a color palette and use that!
            const auto color = not m_enabled ? (has_focus() ? "#A36A6A"_c : "#919191"_c)
                                             : (has_focus()    ? is_hovered() ? "#FF6A00"_c : Color::red()
                                                   : is_hovered() ? "#00BBFF"_c
                                                               : Color::blue());
            service_provider.renderer().draw_rect_filled(m_fill_rect, color);

            m_content.render(service_provider);
        }

        [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override {
            if (not m_enabled) {
                return false;
            }


            const auto navigation_event = input_manager->get_navigation_event(event);

            if (has_focus() and navigation_event == input::NavigationEvent::OK) {
                spdlog::info("Button pressed");
                if (on_clicked()) {
                    return {
                        true,
                        { ui::EventHandleType::RequestAction, this }
                    };
                }
                return true;
            }


            if (const auto hover_result = detect_hover(input_manager, event); hover_result) {
                if (hover_result.is(ActionType::Clicked)) {
                    if (on_clicked()) {
                        return {
                            true,
                            { ui::EventHandleType::RequestAction, this }
                        };
                    }
                }
                return true;
            }

            return false;
        }

        [[nodiscard]] virtual bool on_clicked() const = 0;

        void disable() {
            m_enabled = false;
        }

        void enable() {
            m_enabled = true;
        }

        [[nodiscard]] bool is_enabled() const {
            return m_enabled;
        }
    };

} // namespace ui
