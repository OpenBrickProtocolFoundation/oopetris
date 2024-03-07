
#pragma once

#include "ui/focusable.hpp"
#include "ui/widget.hpp"

namespace ui {

    struct FocusOptions final {
        bool wrap_around;
        bool allow_tab;
    };

    enum class FocusChangeDirection : u8 {
        Forward,
        Backward,
    };

    struct FocusLayout : public Widget, public Focusable, public Hoverable {

    private:
        FocusOptions m_options;

    protected:
        helper::optional<usize> m_focus_id{};
        std::vector<std::unique_ptr<Widget>> m_widgets{};

    public:
        explicit FocusLayout(const Layout& layout, u32 focus_id, FocusOptions options, bool is_top_level)
            : Widget{ layout, WidgetType::Container, is_top_level },
              Focusable{ focus_id },
              Hoverable{ layout.get_rect() },

              m_options{ options } { }


        void update() override {
            for (auto& widget : m_widgets) {
                widget->update();
            }
        }

        [[nodiscard]] u32 widget_count() const {
            return static_cast<u32>(m_widgets.size());
        }


        template<typename T, typename... Args>
        u32 add(Args... args) {
            const u32 index = static_cast<u32>(m_widgets.size());

            const Layout layout = get_layout_for_index(index);

            m_widgets.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)..., layout, false)));
            auto focusable = as_focusable(m_widgets.back().get());
            if (focusable.has_value() and not m_focus_id.has_value()) {
                give_focus(focusable.value());
            }

            return static_cast<u32>(index);
        }


        template<typename T>
        T* get(const u32 index) {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of FocusLayout item: index out of bound!");
            }

            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of FocusLayout item!");
            }

            return item;
        }

        template<typename T>
        const T* get(const u32 index) const {
            if (index >= m_widgets.size()) {
                throw std::runtime_error("Invalid get of FocusLayout item: index out of bound!");
            }

            const auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of FocusLayout item!");
            }

            return item;
        }

    private:
        helper::BoolWrapper<ui::EventHandleType> handle_focus_change_button_events(const SDL_Event& event) {

            helper::BoolWrapper<ui::EventHandleType> handled = false;

            if (utils::device_supports_keys()) {
                if (utils::event_is_action(event, utils::CrossPlatformAction::DOWN)
                    or (m_options.allow_tab and utils::event_is_action(event, utils::CrossPlatformAction::TAB))) {
                    handled = try_set_next_focus(FocusChangeDirection::Forward);
                } else if (utils::event_is_action(event, utils::CrossPlatformAction::UP)) {
                    handled = try_set_next_focus(FocusChangeDirection::Backward);
                }
            }

            return handled;
        }

    protected:
        [[nodiscard]] virtual Layout get_layout_for_index(u32 index) = 0;

        helper::BoolWrapper<ui::EventHandleType>
        handle_focus_change_events(const SDL_Event& event, const Window* window) {


            if (not has_focus() and not is_top_level()) {
                return false;
            }

            helper::BoolWrapper<ui::EventHandleType> handled = false;


            if (m_focus_id.has_value()) {
                const auto& widget = m_widgets.at(focusable_index_by_id(m_focus_id.value()));


                if (widget->type() != WidgetType::Container) {
                    handled = handle_focus_change_button_events(event);
                }


                if (handled) {
                    return handled;
                }

                if (const auto event_result = widget->handle_event(event, window); event_result) {
                    return { true, handle_event_result(event_result.get_additional(), widget.get()) };
                }


                if (widget->type() == WidgetType::Container) {
                    handled = handle_focus_change_button_events(event);
                }
            }


            return handled;
        }

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

                    // try to unfocus, in forward direction, if that fails request unfocus of they container / layout itself, also test backwards, if not wrapping!
                    const auto test_forward = try_set_next_focus(FocusChangeDirection::Forward);
                    if (not test_forward) {
                        if (m_options.wrap_around) {
                            return ui::EventHandleType::RequestUnFocus;
                        }

                        const auto test_backwards = try_set_next_focus(FocusChangeDirection::Backward);
                        if (not test_backwards) {
                            return ui::EventHandleType::RequestUnFocus;
                        }
                    }

                    return helper::nullopt;
                }
                default:
                    std::unreachable();
            }
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

            assert(not focusable_ids.empty() && "if something is focused, we should have at least one focusable widget"
            );

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

        void give_focus(Focusable* focusable) {
            m_focus_id = focusable->focus_id();
            focusable->focus();
        }
    };


} // namespace ui
