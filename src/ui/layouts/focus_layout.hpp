
#pragma once

#include "ui/focusable.hpp"
#include "ui/widget.hpp"

#include "helper/format.hpp"

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
        helper::optional<u32> m_focus_id{};
        std::vector<std::unique_ptr<Widget>> m_widgets{};

    public:
        explicit FocusLayout(const Layout& layout, u32 focus_id, FocusOptions options, bool is_top_level);

        void update() override;

        [[nodiscard]] u32 widget_count() const;


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
        [[nodiscard]] bool is(const u32 index) {
            if (index >= m_widgets.size()) {
                throw std::runtime_error{ fmt::format(
                        "Invalid get of FocusLayout item: index out of bound: {} is not in range [{} - {})", index, 0,
                        m_widgets.size()
                ) };
            }

            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            return item != nullptr;
        }


        template<typename T>
        [[nodiscard]] T* get(const u32 index) {
            if (index >= m_widgets.size()) {
                throw std::runtime_error{ fmt::format(
                        "Invalid get of FocusLayout item: index out of bound: {} is not in range [{} - {})", index, 0,
                        m_widgets.size()
                ) };
            }

            auto item = dynamic_cast<T*>(m_widgets.at(index).get());
            if (item == nullptr) {
                throw std::runtime_error("Invalid get of FocusLayout item!");
            }

            return item;
        }

        template<typename T>
        [[nodiscard]] const T* get(const u32 index) const {
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
        Widget::EventHandleResult handle_focus_change_button_events(const SDL_Event& event);

    protected:
        [[nodiscard]] virtual Layout get_layout_for_index(u32 index) = 0;

        Widget::EventHandleResult handle_focus_change_events(const SDL_Event& event, const Window* window);

        [[nodiscard]] helper::optional<ui::Widget::InnerState>
        handle_event_result(const helper::optional<ui::Widget::InnerState>& result, Widget* widget);

        [[nodiscard]] u32 focusable_index_by_id(u32 id) const;

        [[nodiscard]] std::vector<u32> focusable_ids_sorted() const;

        [[nodiscard]] static u32 index_of(const std::vector<u32>& ids, u32 needle);

        [[nodiscard]] bool try_set_next_focus(FocusChangeDirection focus_direction);

        void give_focus(Focusable* focusable);
    };


} // namespace ui
