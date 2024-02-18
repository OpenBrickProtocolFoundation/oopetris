#pragma once


#include "focusable.hpp"
#include "graphics/rect.hpp"
#include "graphics/texture.hpp"
#include "helper/optional.hpp"
#include "helper/types.hpp"
#include "platform/capabilities.hpp"
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


    struct ScrollLayout : public Widget {
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
        std::uint64_t current_scroll_postion;
        Layout main_layout;
        Layout scroll_bar_layout;

    public:
        explicit ScrollLayout(
                ServiceProvider* service_provider,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout
        )
            : Widget{ layout },
              gap{ gap },
              m_texture{ service_provider->renderer().get_texture_for_render_target(
                      shapes::Point(1, 1) // this is a dummy point!
              ) },
              m_service_provider{ service_provider },
              current_scroll_postion{ 0 },
              main_layout{ layout },
              scroll_bar_layout{ layout } {

            const auto layout_rect = layout.get_rect();
            const auto absolut_margin = std::pair<u32, u32>{ static_cast<u32>(margin.first * layout_rect.width()),
                                                             static_cast<u32>(margin.second * layout_rect.height()) };


            const auto scroll_bar_width = static_cast<u32>(0.02 * layout_rect.width());

            const auto start_x = layout_rect.top_left.x + absolut_margin.first;
            const auto start_y = layout_rect.top_left.y + absolut_margin.second;

            const auto new_width = layout_rect.width() - absolut_margin.first * 2;
            const auto new_height = layout_rect.height() - absolut_margin.second * 2;


            main_layout = AbsolutLayout{ start_x, start_y, new_width - scroll_bar_width, new_height };
            scroll_bar_layout =
                    AbsolutLayout{ start_x + new_width - scroll_bar_width, start_y, scroll_bar_width, new_height };
        }

        [[nodiscard]] u32 widget_count() const {
            return m_widgets.size();
        }

        void render(const ServiceProvider& service_provider) const override {

            const auto& renderer = service_provider.renderer();

            // at widget_count == 0, the texture is a dummy, so don#t use it!
            if (widget_count() > 0) {

                renderer.set_render_target(m_texture);
                renderer.clear();
                for (const auto& widget : m_widgets) {
                    widget->render(service_provider);
                }

                renderer.reset_render_target();

                const auto texture_size = m_texture.size();

                //TODO: render texture correctly
                const auto from = shapes::Rect{ 0, 0, texture_size.x, texture_size.y };

                renderer.draw_texture(m_texture, from, main_layout.get_rect());
            }


            renderer.draw_rect_filled(scroll_bar_layout.get_rect(), Color(0xA1, 0X97, 0x97));

            //TODO render scroll bar
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
        void add(ItemSize size, Args... args) {

            const Layout layout = get_layout_for_new(size);

            m_widgets.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)..., layout)));
            auto focusable = as_focusable(*m_widgets.back());
            if (focusable.has_value() and not m_focus_id.has_value()) {
                give_focus(focusable.value());
            }

            //TODO: this is not entirely correct (check some cases, that might occur!), it might be also off by 1!
            const auto total_size = layout.get_rect().bottom_right;


            m_texture = m_service_provider->renderer().get_texture_for_render_target(total_size);
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
            auto start_point = shapes::Point::zero();

            for (const auto& widget : m_widgets) {
                const auto widget_rect = widget->layout().get_rect();
                start_point.y += widget_rect.height() + gap.get_margin();
            }

            const auto height = size.get_height();
            const auto width = static_cast<u32>(main_layout.get_rect().height());

            return AbsolutLayout{
                static_cast<u32>(start_point.x),
                static_cast<u32>(start_point.y),
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
