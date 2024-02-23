#pragma once


#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"
#include "helper/optional.hpp"
#include "helper/types.hpp"
#include "platform/capabilities.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"

#include <vector>

namespace ui {

    enum class ItemSizeType { Relative, Absolut };

    struct ItemSize {
    private:
        u32 height;
        ItemSizeType type;

    protected:
        ItemSize(const u32 height, ItemSizeType type) : height{ height }, type{ type } { }

    public:
        [[nodiscard]] u32 get_height() const {
            return height;
        }

        [[nodiscard]] ItemSizeType get_type() const {
            return type;
        }
    };

    struct AbsolutItemSize : public ItemSize {
        AbsolutItemSize(const u32 height) : ItemSize{ height, ItemSizeType::Absolut } { }
    };


    struct RelativeItemSize : public ItemSize {
        RelativeItemSize(const shapes::Rect& rect, const double height)
            : ItemSize{ static_cast<u32>(height * rect.height()), ItemSizeType::Relative } {
            // no checks for upper cases, since it theoretically can also be larger than the whole screen!
            assert(height >= 0.0 && "height has to be in correct percentage range!");
        }
        RelativeItemSize(const Window* window, const double height)
            : RelativeItemSize{ window->screen_rect(), height } { }
        RelativeItemSize(const Window& window, const double height)
            : RelativeItemSize{ window.screen_rect(), height } { }
        RelativeItemSize(const Layout& layout, const double height) : RelativeItemSize{ layout.get_rect(), height } { }
    };


    struct ScrollLayout : public Widget, public Focusable, public Hoverable {
    private:
        enum class FocusChangeDirection {
            Forward,
            Backward,
        };

        std::vector<std::unique_ptr<Widget>> m_widgets{};
        helpers::optional<usize> m_focus_id;
        Margin gap;
        Texture m_texture;
        ServiceProvider* m_service_provider;
        shapes::Rect main_rect;
        shapes::Rect scrollbar_rect;
        shapes::Rect scrollbar_mover_rect;
        shapes::Rect m_viewport;
        bool is_dragging{ false };
        u32 m_step_size;

    public:
        explicit ScrollLayout(
                ServiceProvider* service_provider,
                usize focus_id,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout
        )
            : Widget{ layout },
              Focusable{ focus_id },
              Hoverable{ layout.get_rect() },
              gap{ gap },
              m_texture{ service_provider->renderer().get_texture_for_render_target(
                      shapes::Point(1, 1) // this is a dummy point!
              ) },
              m_service_provider{ service_provider },
              m_step_size{ static_cast<u32>(layout.get_rect().height() * 0.05) } {

            const auto layout_rect = layout.get_rect();
            const auto absolut_margin = std::pair<u32, u32>{ static_cast<u32>(margin.first * layout_rect.width()),
                                                             static_cast<u32>(margin.second * layout_rect.height()) };

            constexpr u32 absolut_gap = 10;

            const auto scroll_bar_width = static_cast<u32>(0.02 * layout_rect.width());

            const auto start_x = layout_rect.top_left.x + absolut_margin.first;
            const auto start_y = layout_rect.top_left.y + absolut_margin.second;

            const auto new_width = layout_rect.width() - absolut_margin.first * 2;
            const auto new_height = layout_rect.height() - absolut_margin.second * 2;


            main_rect = shapes::Rect{ static_cast<int>(start_x), static_cast<int>(start_y),
                                      static_cast<int>(new_width - scroll_bar_width - absolut_gap),
                                      static_cast<int>(new_height) };
            scrollbar_rect =
                    shapes::Rect{ static_cast<int>(start_x + new_width - scroll_bar_width), static_cast<int>(start_y),
                                  static_cast<int>(scroll_bar_width), static_cast<int>(new_height) };
            scrollbar_mover_rect = scrollbar_rect; // NOLINT(cppcoreguidelines-prefer-member-initializer)
            m_viewport = shapes::Rect{ 0, 0, 0, 0 };
        }

        [[nodiscard]] u32 widget_count() const {
            return static_cast<u32>(m_widgets.size());
        }

        void render(const ServiceProvider& service_provider) const override {

            const auto& renderer = service_provider.renderer();

            const auto total_widgets_height = m_widgets.back()->layout().get_rect().bottom_right.y;

            // at widget_count == 0, the texture is a dummy, so don't use it!
            if (widget_count() > 0) {

                renderer.set_render_target(m_texture);
                renderer.clear();
                for (const auto& widget : m_widgets) {
                    // smart rendering, only render, when viewport needs this widget
                    const auto layout_rect = widget->layout().get_rect();
                    if (layout_rect.top_left.y > m_viewport.bottom_right.y) {
                        continue;
                    }

                    if (layout_rect.bottom_right.y < m_viewport.top_left.y) {
                        continue;
                    }

                    widget->render(service_provider);
                }

                renderer.reset_render_target();

                auto to_rect = main_rect;
                // if we don't need to fill-up the whole main_rect, we need a special to_rect
                if (total_widgets_height < scrollbar_rect.height()) {
                    to_rect = shapes::Rect{ main_rect.top_left.x, main_rect.top_left.y, main_rect.width(),
                                            total_widgets_height };
                }

                renderer.draw_texture(m_texture, m_viewport, to_rect);
            }

            // render the scrollbar when it makes sense
            if (total_widgets_height > scrollbar_rect.height()) {
                renderer.draw_rect_filled(scrollbar_rect, Color(0xA1, 0X97, 0x97));
                renderer.draw_rect_filled(
                        scrollbar_mover_rect, is_dragging ? Color(0x66, 0x61, 0x61) : Color(0x52, 0x4F, 0x4F)
                );
            }
        }

        bool
        handle_event(const SDL_Event& event, const Window* window) // NOLINT(readability-function-cognitive-complexity)
                override {

            auto handled = false;

            // can't use tab and doesn't cycle, since this is not a fully focusable ui component! (normally only components and not layouts are focusable)

            if (utils::device_supports_keys() and has_focus()) {
                if (utils::event_is_action(event, utils::CrossPlatformAction::DOWN)) {
                    handled = try_set_next_focus(FocusChangeDirection::Forward);
                } else if (utils::event_is_action(event, utils::CrossPlatformAction::UP)) {
                    handled = try_set_next_focus(FocusChangeDirection::Backward);
                }
            }

            if (handled) {
                auto_move_after_focus_change();
                return true;
            }

            if (utils::device_supports_clicks()) {

                const auto total_widgets_height = m_widgets.back()->layout().get_rect().bottom_right.y;

                const auto change_value_on_scroll = [&window, &event, total_widgets_height, this]() {
                    const auto& [_, y] = utils::get_raw_coordinates(window, event);

                    auto desired_scroll_height = 0;


                    if (y <= scrollbar_rect.top_left.y) {
                        desired_scroll_height = 0;
                    } else if (y >= scrollbar_rect.bottom_right.y) {
                        // this is to high, but recalculate_sizes reset it to the highest possible value!
                        desired_scroll_height = total_widgets_height;
                    } else {

                        const double percentage = static_cast<double>(y - scrollbar_rect.top_left.y)
                                                  / static_cast<double>(scrollbar_rect.height());

                        // we want the final point to be in the middle, but desired_scroll_height expects the top position.
                        desired_scroll_height =
                                static_cast<int>(percentage * total_widgets_height) - scrollbar_rect.height() / 2;
                        is_dragging = true;
                    }


                    recalculate_sizes(desired_scroll_height);
                };


                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonDown)) {
                    // note: this behaviour is intentional, namely, clicking into the scroll slider doesn't move it, it just "grabs" it for dragging
                    if (utils::is_event_in(window, event, scrollbar_mover_rect)) {
                        is_dragging = true;
                        handled = true;
                    } else if (utils::is_event_in(window, event, scrollbar_rect)) {

                        change_value_on_scroll();
                        handled = true;
                    }

                } else if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::ButtonUp)) {
                    is_dragging = false;
                    handled = true;

                } else if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Motion)) {

                    if (is_dragging) {

                        change_value_on_scroll();
                        handled = true;
                    }
                } else if (event.type == SDL_MOUSEWHEEL) {

                    // attention the mouse direction changes (it's called natural scrolling on macos/ windows / linux) are not detected by sdl until restart, and here we use the correct scroll behaviour, as teh user configured the mouse in it's OS
                    const bool direction_is_down =
                            event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? event.wheel.y < 0 : event.wheel.y > 0;


                    auto desired_scroll_height = 0;

                    if (direction_is_down) {
                        desired_scroll_height = m_viewport.top_left.y + static_cast<int>(m_step_size);
                    } else {
                        desired_scroll_height = m_viewport.top_left.y - static_cast<int>(m_step_size);
                    }

                    recalculate_sizes(desired_scroll_height);
                    handled = true;
                }

                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {

                    const auto offset_distance = main_rect.top_left - m_viewport.top_left;
                    for (auto& widget : m_widgets) {
                        const auto& layout_rect = widget->layout().get_rect();
                        const auto& offset_rect = layout_rect.move(offset_distance);

                        if (not handled and utils::is_event_in(window, event, main_rect) and utils::is_event_in(window, event, offset_rect)) {
                            const auto offset_event = utils::offset_event(window, event, -offset_distance);

                            if (widget->handle_event(offset_event, window)) {
                                handled = true;
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
            }

            if (not has_focus()) {
                return false;
            }

            if (m_focus_id.has_value()) {
                const auto& widget = m_widgets.at(focusable_index_by_id(m_focus_id.value()));
                if (widget->handle_event(event, window)) {
                    return true;
                }
            }
            return false;
        }

        template<typename T, typename... Args>
        void add(ItemSize size, Args... args) {

            const Layout layout = get_layout_for_new(size);

            m_widgets.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)..., layout)));
            auto focusable = as_focusable(m_widgets.back().get());
            if (focusable.has_value() and not m_focus_id.has_value()) {
                give_focus(focusable.value());
            }

            const auto total_size = layout.get_rect().bottom_right;

            m_texture = m_service_provider->renderer().get_texture_for_render_target(total_size);
            recalculate_sizes(0);
        }

        template<typename T>
        T* get(const u32 index) {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of ScrollLayout item: index out of bound!");
            }

            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of ScrollLayout item!");
            }

            return item;
        }

        template<typename T>
        const T* get(const u32 index) const {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of ScrollLayout item: index out of bound!");
            }

            const auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of ScrollLayout item!");
            }

            return item;
        }

    private:
        [[nodiscard]] Layout get_layout_for_new(ItemSize size) {
            u32 start_point_y = 0;

            for (const auto& widget : m_widgets) {
                const auto& widget_rect = widget->layout().get_rect();
                start_point_y += static_cast<u32>(widget_rect.height());
            }

            start_point_y += gap.get_margin() * static_cast<u32>(m_widgets.size());

            const auto width = static_cast<u32>(main_rect.width());
            const auto height = size.get_height();

            return AbsolutLayout{
                0,
                start_point_y,
                width,
                height,
            };
        }


        void auto_move_after_focus_change() {

            if (not m_focus_id.has_value()) {
                return;
            }

            const auto total_widgets_height = m_widgets.back()->layout().get_rect().bottom_right.y;

            // if we don't need to fill-up the whole main_rect, we need a special viewport, but top position is always 0
            if (total_widgets_height < scrollbar_rect.height()) {
                recalculate_sizes(0);
                return;
            }

            // we center the in focus element (if possible -> not on top or bottom)

            const auto& widget = m_widgets.at(focusable_index_by_id(m_focus_id.value()));

            const auto widget_rect = widget->layout().get_rect();

            // determine if the middle is +- (1 % px) in the viewport middle
            const auto middle_of_rect_y = widget_rect.top_left.y + (widget_rect.height() / 2);


            const auto viewport_middle_y = m_viewport.top_left.y + (m_viewport.height() / 2);

            const auto is_circa_in_middle =
                    std::abs(middle_of_rect_y - viewport_middle_y)
                    <= static_cast<int>(m_service_provider->window().screen_rect().height() * 0.05);

            if (is_circa_in_middle) {
                return;
            }

            recalculate_sizes(middle_of_rect_y - (m_viewport.height() / 2));
        }

        // it's called desired, since it might not be entirely valid
        void recalculate_sizes(i32 desired_scroll_height) {

            const auto total_widgets_height = m_widgets.back()->layout().get_rect().bottom_right.y;

            // if we don't need to fill-up the whole main_rect, we need a special viewport
            if (total_widgets_height < scrollbar_rect.height()) {
                m_viewport = shapes::Rect{ 0, 0, main_rect.width(), total_widgets_height };
            }

            // check if desired_scroll_height is valid:
            auto scroll_height = desired_scroll_height;

            if (desired_scroll_height < 0) {
                scroll_height = 0;
            } else if (desired_scroll_height + main_rect.height() > total_widgets_height) {
                scroll_height = total_widgets_height - main_rect.height();
            }

            m_viewport = shapes::Rect{ 0, scroll_height, main_rect.width(), main_rect.height() };


            // recalculate scrollbar mover rect
            const auto current_start_height = static_cast<u32>(
                    scrollbar_rect.height()
                    * (static_cast<double>(m_viewport.top_left.y) / static_cast<double>(total_widgets_height))
            );

            const auto current_end_height = static_cast<u32>(
                    scrollbar_rect.height()
                    * (static_cast<double>(m_viewport.top_left.y + scrollbar_rect.height())
                       / static_cast<double>(total_widgets_height))
            );

            scrollbar_mover_rect =
                    shapes::Rect{ scrollbar_rect.top_left.x,
                                  static_cast<int>(scrollbar_rect.top_left.y + current_start_height),
                                  scrollbar_rect.width(), static_cast<int>(current_end_height - current_start_height) };
        }

        void give_focus(Focusable* focusable) {
            m_focus_id = focusable->focus_id();
            focusable->focus();
        }

        //TODO: this was copied a few times, deduplicate it
        [[nodiscard]] usize focusable_index_by_id(const usize id) {
            const auto find_iterator =
                    std::find_if(m_widgets.begin(), m_widgets.end(), [id](const std::unique_ptr<Widget>& widget) {
                        const auto focusable = as_focusable(widget.get());
                        return focusable.has_value() and focusable.value()->focus_id() == id;
                    });
            assert(find_iterator != m_widgets.end());
            const auto index = static_cast<usize>(std::distance(m_widgets.begin(), find_iterator));
            return index;
        }

        [[nodiscard]] std::vector<usize> focusable_ids_sorted() const {
            auto result = std::vector<usize>{};
            for (const auto& widget : m_widgets) {
                const auto focusable = as_focusable(widget.get());
                if (focusable) {
                    result.push_back((*focusable)->focus_id());
                }
            }

#ifdef DEBUG_BUILD
            const auto duplicates = std::adjacent_find(result.cbegin(), result.cend());
            if (duplicates != result.cend()) {
                throw std::runtime_error("Focusables have duplicates: " + std::to_string(*duplicates));
            }
#endif
            std::sort(result.begin(), result.end());
            return result;
        }

        [[nodiscard]] static usize index_of(const std::vector<usize>& ids, const usize needle) {
            return static_cast<usize>(std::distance(ids.cbegin(), std::find(ids.cbegin(), ids.cend(), needle)));
        }

        [[nodiscard]] bool try_set_next_focus(const FocusChangeDirection focus_direction) {

            if (not m_focus_id.has_value()) {
                return false;
            }

            const auto focusable_ids = focusable_ids_sorted();

            assert(not focusable_ids.empty());
            const int current_index = static_cast<int>(index_of(focusable_ids, m_focus_id.value()));
            const int next_index =
                    (focus_direction == FocusChangeDirection::Forward ? current_index + 1 : current_index - 1);

            if (next_index < 0) {
                return false;
            }

            if (next_index >= static_cast<int>(focusable_ids.size())) {
                return false;
            }

            auto current_focusable =
                    as_focusable(m_widgets.at(focusable_index_by_id(focusable_ids.at(current_index))).get());
            assert(current_focusable.has_value());
            auto next_focusable = as_focusable(m_widgets.at(focusable_index_by_id(focusable_ids.at(next_index))).get());
            assert(next_focusable.has_value());
            current_focusable.value()->unfocus();            // NOLINT(bugprone-unchecked-optional-access)
            next_focusable.value()->focus();                 // NOLINT(bugprone-unchecked-optional-access)
            m_focus_id = next_focusable.value()->focus_id(); // NOLINT(bugprone-unchecked-optional-access)

            return true;
        }
    };

} // namespace ui
