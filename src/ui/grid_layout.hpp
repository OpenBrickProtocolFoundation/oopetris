#pragma once

#include <algorithm>
#include <memory>
#include <tl/optional.hpp>
#include <vector>

#include "../capabilities.hpp"
#include "../rect.hpp"
#include "../types.hpp"
#include "focusable.hpp"
#include "widget.hpp"

namespace ui {

    enum class Direction { Horizontal, Vertical };

    struct Margin {
    private:
        u32 m_margin;

    public:
        Margin(u32 margin) : m_margin{ margin } { }


        [[nodiscard]] u32 get_margin() const {
            return m_margin;
        }
    };

    struct AbsolutMargin : public Margin {
        AbsolutMargin(const u32 margin) : Margin{ margin } { }
    };

    struct RelativeMargin : public Margin {
        RelativeMargin(const Rect& rect, Direction direction, const double margin)
            : Margin{ static_cast<u32>(margin * (direction == Direction::Horizontal ? rect.width() : rect.height())) } {

            assert(margin >= 0.0 && margin <= 1.0 && "margin has to be in correct percentage range!");
        }
        RelativeMargin(const Window& window, Direction direction, const double margin)
            : RelativeMargin{ window.screen_rect(), direction, margin } { }
        RelativeMargin(const Window* window, Direction direction, const double margin)
            : RelativeMargin{ window->screen_rect(), direction, margin } { }
    };


    template<size_t S>
    struct GridLayout : public Widget {
    private:
        enum class FocusChangeDirection {
            Forward,
            Backward,
        };

        std::array<std::unique_ptr<Widget>, S> m_widgets;
        tl::optional<usize> m_focus_id;
        Direction direction;
        Margin gap;
        std::pair<u32, u32> margin;

    public:
        explicit GridLayout(
                const Layout& layout,
                Direction direction = Direction::Vertical,
                Margin gap = AbsolutMargin{ 0 },
                std::pair<double, double> margin = std::pair<u32, u32>{ 0.0, 0.0 }
        )
            : Widget(layout),
              m_widgets{},
              direction{ direction },
              gap{ gap },
              margin{ static_cast<u32>(margin.first * layout.get_rect().width()),
                      static_cast<u32>(margin.second * layout.get_rect().height()) } { }

        void render(const ServiceProvider& service_provider) const override {
            for (const auto& widget : m_widgets) {
                widget->render(service_provider);
            }
        }

        bool handle_event(const SDL_Event& event, const Window* window) override {
            auto handled = false;
            if (utils::device_supports_keys()) {
                if (utils::event_is_action(event, utils::CrossPlatformAction::DOWN)) {
                    handled = try_set_next_focus(FocusChangeDirection::Forward);
                } else if (utils::event_is_action(event, utils::CrossPlatformAction::UP)) {
                    handled = try_set_next_focus(FocusChangeDirection::Backward);
                } else if (utils::event_is_action(event, utils::CrossPlatformAction::TAB)) {
                    handled = try_set_next_focus(FocusChangeDirection::Forward);
                }
            }

            if (handled) {
                return true;
            }

            if (utils::device_supports_clicks()) {

                if (utils::event_is_click_event(event, utils::CrossPlatformClickEvent::Any)) {

                    for (usize i = 0; i < m_widgets.size(); ++i) {
                        const auto layout = get_layout_for_index(i);
                        if (utils::is_event_in(window, event, layout.get_rect())) {
                            if (m_widgets.at(i)->handle_event(event, window)) {
                                return true;
                            }
                        }
                    }
                }
            }


            for (auto& widget : m_widgets) {
                if (widget->handle_event(event, window)) {
                    return true;
                }
            }
            return false;
        }

        template<typename T, typename... Args>
        void add(const size_t index, Args... args) {

            const Layout layout = get_layout_for_index(index);

            m_widgets.at(index) = std::move(std::make_unique<T>(std::forward<Args>(args)..., layout));
            auto focusable = as_focusable(*m_widgets.at(index));
            if (focusable.has_value() and not m_focus_id.has_value()) {
                give_focus(focusable.value());
            }
        }


        template<typename T>
        T* get(const size_t index) {
            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of GridLayout item!");
            }

            return item;
        }

        template<typename T>
        const T* get(const size_t index) const {
            const auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of GridLayout item!");
            }

            return item;
        }

    private:
        [[nodiscard]] Layout get_layout_for_index(size_t index) {
            const auto start_point = layout.get_rect().top_left;

            u32 x = start_point.x + margin.first;
            u32 y = start_point.y + margin.second;
            u32 width = layout.get_rect().width() - (margin.first * 2);
            u32 height = layout.get_rect().height() - (margin.second * 2);

            if (direction == Direction::Horizontal) {
                u32 total_margin = S <= 1 ? 0 : (S - 1) * gap.get_margin();
                width = (layout.get_rect().width() - total_margin - (margin.first * 2)) / S;

                u32 margin_x = index * gap.get_margin();
                u32 total_width = width * index;
                x += margin_x + total_width;
            } else {
                u32 total_margin = S <= 1 ? 0 : (S - 1) * gap.get_margin();
                height = (layout.get_rect().height() - total_margin - (margin.second * 2)) / S;

                u32 margin_y = index * gap.get_margin();
                u32 total_height = height * index;
                y += margin_y + total_height;
            }


            return AbsolutLayout{
                x,
                y,
                width,
                height,
            };
        }

        void give_focus(Focusable& focusable) {
            m_focus_id = focusable.focus_id();
            focusable.focus();
        }

        [[nodiscard]] static tl::optional<Focusable&> as_focusable(Widget& widget) {
            const auto focusable = dynamic_cast<Focusable*>(&widget);
            if (focusable == nullptr) {
                return tl::nullopt;
            }

            return *focusable;
        }

        [[nodiscard]] usize focusable_index_by_id(const usize id) {
            const auto find_iterator =
                    std::find_if(m_widgets.begin(), m_widgets.end(), [id](const std::unique_ptr<Widget>& widget) {
                        const auto focusable = as_focusable(*widget);
                        return focusable.has_value() and focusable.value().focus_id() == id;
                    });
            assert(find_iterator != m_widgets.end());
            const auto index = static_cast<usize>(std::distance(m_widgets.begin(), find_iterator));
            return index;
        }

        [[nodiscard]] std::vector<usize> focusable_ids_sorted() const {
            auto result = std::vector<usize>{};
            for (const auto& widget : m_widgets) {
                const auto focusable = as_focusable(*widget);
                if (focusable) {
                    result.push_back(focusable->focus_id());
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

        [[nodiscard]] bool try_set_next_focus(const FocusChangeDirection direction) {

            if (not m_focus_id.has_value()) {
                return false;
            }

            const auto focusable_ids = focusable_ids_sorted();

            assert(not focusable_ids.empty());
            const auto current_index = index_of(focusable_ids, m_focus_id.value());
            const auto next_index =
                    (direction == FocusChangeDirection::Forward
                             ? ((current_index + 1) % focusable_ids.size())
                             : ((current_index + focusable_ids.size() - 1) % focusable_ids.size()));

            auto current_focusable =
                    as_focusable(*m_widgets.at(focusable_index_by_id(focusable_ids.at(current_index))));
            assert(current_focusable.has_value());
            auto next_focusable = as_focusable(*m_widgets.at(focusable_index_by_id(focusable_ids.at(next_index))));
            assert(next_focusable.has_value());
            current_focusable->unfocus();
            next_focusable->focus();
            m_focus_id = next_focusable->focus_id();

            return true;
        }
    };

} // namespace ui
