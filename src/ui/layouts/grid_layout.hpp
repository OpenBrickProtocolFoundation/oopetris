#pragma once


#include "graphics/rect.hpp"
#include "helper/optional.hpp"
#include "helper/types.hpp"
#include "platform/capabilities.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/widget.hpp"

#include <vector>

namespace ui {
    struct GridLayout : public Widget, public Hoverable {
    private:
        enum class FocusChangeDirection {
            Forward,
            Backward,
        };

        std::vector<std::unique_ptr<Widget>> m_widgets{};
        u32 size;
        helper::optional<usize> m_focus_id;
        Direction direction;
        Margin gap;
        std::pair<u32, u32> margin;

    public:
        explicit GridLayout(
                u32 size,
                Direction direction,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout
        )
            : Widget{ layout },
              Hoverable{ layout.get_rect() },
              size{ size },
              direction{ direction },
              gap{ gap },
              margin{ static_cast<u32>(margin.first * layout.get_rect().width()),
                      static_cast<u32>(margin.second * layout.get_rect().height()) } { }

        [[nodiscard]] u32 widget_count() const {
            return size;
        }

        void render(const ServiceProvider& service_provider) const override {
            for (const auto& widget : m_widgets) {
                assert(widget.get() != nullptr && "Grid Layout initalized with to few child widgets!");
                widget->render(service_provider);
            }
        }

        helper::BoolWrapper<ui::EventHandleType>
        handle_event(const SDL_Event& event, const Window* window) // NOLINT(readability-function-cognitive-complexity)
                override {
            helper::BoolWrapper<ui::EventHandleType> handled = false;

            if (utils::device_supports_keys()) {
                if (utils::event_is_action(event, utils::CrossPlatformAction::DOWN)
                    || utils::event_is_action(event, utils::CrossPlatformAction::TAB)) {
                    handled = try_set_next_focus(FocusChangeDirection::Forward);
                } else if (utils::event_is_action(event, utils::CrossPlatformAction::UP)) {
                    handled = try_set_next_focus(FocusChangeDirection::Backward);
                }
            }

            if (handled) {
                return true;
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
                            const auto hoverable = as_hoverable(widget.get());
                            if (hoverable.has_value()) {
                                hoverable.value()->on_unhover();
                            }
                        }
                    }
                    return handled;
                }
            }

            if (m_focus_id.has_value()) {
                const auto& widget = m_widgets.at(focusable_index_by_id(m_focus_id.value()));
                if (const auto event_result = widget->handle_event(event, window); event_result) {
                    return { true, handle_event_result(event_result.get_additional(), widget.get()) };
                }
            }

            return false;
        }

        template<typename T, typename... Args>
        u32 add(Args... args) {
            const u32 index = static_cast<u32>(m_widgets.size());

            if (index >= this->size) {
                throw std::runtime_error("GridLayout is already full");
            }

            const Layout layout = get_layout_for_index(index);

            m_widgets.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)..., layout)));
            auto focusable = as_focusable(m_widgets.back().get());
            if (focusable.has_value() and not m_focus_id.has_value()) {
                give_focus(focusable.value());
            }

            return static_cast<u32>(index);
        }


        template<typename T>
        T* get(const u32 index) {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of GridLayout item: index out of bound!");
            }

            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of GridLayout item!");
            }

            return item;
        }

        template<typename T>
        const T* get(const u32 index) const {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of GridLayout item: index out of bound!");
            }

            const auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of GridLayout item!");
            }

            return item;
        }

    private:
        [[nodiscard]] helper::optional<ui::EventHandleType>
        handle_event_result(const helper::optional<ui::EventHandleType>& result, Widget* widget) {

            if (not result.has_value()) {
                return helper::nullopt;
            }

            switch (result.value()) {
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

                    auto current_focusable =
                            as_focusable(m_widgets.at(focusable_index_by_id(m_focus_id.value())).get());
                    assert(current_focusable.has_value());
                    current_focusable.value()->unfocus(); // NOLINT(bugprone-unchecked-optional-access)
                    focusable.value()->focus();           // NOLINT(bugprone-unchecked-optional-access)
                    m_focus_id = widget_focus_id;

                    return helper::nullopt;
                }
                case ui::EventHandleType::RequestUnFocus: {
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

                    const auto test_forward = try_set_next_focus(FocusChangeDirection::Forward);
                    if (not test_forward) {
                        return ui::EventHandleType::RequestUnFocus;
                    }

                    return helper::nullopt;
                }
                default:
                    std::unreachable();
            }
        }

        [[nodiscard]] Layout get_layout_for_index(u32 index) {
            const auto start_point = layout().get_rect().top_left;

            u32 x = start_point.x + margin.first;
            u32 y = start_point.y + margin.second;
            u32 width = layout().get_rect().width() - (margin.first * 2);
            u32 height = layout().get_rect().height() - (margin.second * 2);

            if (direction == Direction::Horizontal) {
                const u32 total_margin = this->size <= 1 ? 0 : (this->size - 1) * gap.get_margin();
                width = (layout().get_rect().width() - total_margin - (margin.first * 2)) / this->size;

                const u32 margin_x = index * gap.get_margin();
                const u32 total_width = width * index;
                x += margin_x + total_width;
            } else {
                const u32 total_margin = this->size <= 1 ? 0 : (this->size - 1) * gap.get_margin();
                height = (layout().get_rect().height() - total_margin - (margin.second * 2)) / this->size;

                const u32 margin_y = index * gap.get_margin();
                const u32 total_height = height * index;
                y += margin_y + total_height;
            }


            return AbsolutLayout{
                x,
                y,
                width,
                height,
            };
        }

        void give_focus(Focusable* focusable) {
            m_focus_id = focusable->focus_id();
            focusable->focus();
        }

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

            if (focusable_ids.size() == 1) {
                return false;
            }

            const auto current_index = index_of(focusable_ids, m_focus_id.value());
            const auto next_index =
                    (focus_direction == FocusChangeDirection::Forward
                             ? ((current_index + 1) % focusable_ids.size())
                             : ((current_index + focusable_ids.size() - 1) % focusable_ids.size()));

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
