#pragma once


#include "focusable.hpp"
#include "graphics/renderer.hpp"
#include "helper/types.hpp"
#include "ui/grid_layout.hpp"
#include "ui/widget.hpp"

#include <array>

namespace ui {

    template<size_t S>
    struct TileLayout : public Widget {
    private:
        enum class FocusChangeDirection {
            Forward,
            Backward,
        };

        std::array<std::unique_ptr<Widget>, S> m_widgets{};
        helpers::optional<usize> m_focus_id;
        Direction direction;
        std::array<double, S - 1> steps;
        Margin gap;
        std::pair<u32, u32> margin;

    public:
        explicit TileLayout(
                Direction direction,
                std::array<double, S - 1> steps,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout
        )
            : Widget{ layout },
              direction{ direction },
              steps{ steps },
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
                        if (utils::is_event_in(window, event, layout.get_rect())) {
                            if (widget->handle_event(event, window)) {
                                return true;
                            }
                        }
                    }

                    return false;
                }
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
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of TileLayout item: index out of bound!");
            }

            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of TileLayout item!");
            }

            return item;
        }

        template<typename T>
        const T* get(const size_t index) const {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of TileLayout item: index out of bound!");
            }

            const auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of TileLayout item!");
            }

            return item;
        }

    private:
        [[nodiscard]] Layout get_layout_for_index(size_t index) {
            const auto start_point = layout().get_rect().top_left;

            u32 x = start_point.x + margin.first;
            u32 y = start_point.y + margin.second;
            u32 width = layout().get_rect().width() - (margin.first * 2);
            u32 height = layout().get_rect().height() - (margin.second * 2);

            if (direction == Direction::Horizontal) {
                const auto previous_start =
                        index == 0 ? 0 : static_cast<u32>(width * steps.at(index - 1)) + gap.get_margin() / 2;

                const auto current_end =
                        index == S - 1 ? width
                                       : (steps.size() <= index
                                                  ? width
                                                  : static_cast<u32>(width * steps.at(index)) - gap.get_margin() / 2);

                width = current_end - previous_start;
                x += previous_start;
            } else {
                const auto previous_start =
                        index == 0 ? 0 : static_cast<u32>(height * steps.at(index - 1)) + gap.get_margin() / 2;

                const auto current_end =
                        index == S - 1 ? height
                                       : (steps.size() <= index
                                                  ? height
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

        void give_focus(Focusable* focusable) {
            m_focus_id = focusable->focus_id();
            focusable->focus();
        }

        [[nodiscard]] static helpers::optional<Focusable*> as_focusable(Widget& widget) {
            auto* const focusable = dynamic_cast<Focusable*>(&widget);
            if (focusable == nullptr) {
                return helpers::nullopt;
            }

            return focusable;
        }

        [[nodiscard]] usize focusable_index_by_id(const usize id) {
            const auto find_iterator =
                    std::find_if(m_widgets.begin(), m_widgets.end(), [id](const std::unique_ptr<Widget>& widget) {
                        const auto focusable = as_focusable(*widget);
                        return focusable.has_value() and focusable.value()->focus_id() == id;
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
            const auto current_index = index_of(focusable_ids, m_focus_id.value());
            const auto next_index =
                    (focus_direction == FocusChangeDirection::Forward
                             ? ((current_index + 1) % focusable_ids.size())
                             : ((current_index + focusable_ids.size() - 1) % focusable_ids.size()));

            auto current_focusable =
                    as_focusable(*m_widgets.at(focusable_index_by_id(focusable_ids.at(current_index))));
            assert(current_focusable.has_value());
            auto next_focusable = as_focusable(*m_widgets.at(focusable_index_by_id(focusable_ids.at(next_index))));
            assert(next_focusable.has_value());
            (*current_focusable)->unfocus();
            (*next_focusable)->focus();
            m_focus_id = (*next_focusable)->focus_id();

            return true;
        }
    };

} // namespace ui
