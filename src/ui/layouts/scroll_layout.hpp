#pragma once


#include "focus_layout.hpp"
#include "graphics/rect.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"
#include "helper/optional.hpp"
#include "helper/types.hpp"
#include "platform/capabilities.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"

#include <vector>

namespace ui {

    enum class ItemSizeType : u8 { Relative, Absolut };

    struct ItemSize {
    private:
        u32 height;
        ItemSizeType type;

    protected:
        ItemSize(const u32 height, ItemSizeType type);

    public:
        [[nodiscard]] u32 get_height() const;

        [[nodiscard]] ItemSizeType get_type() const;
    };

    struct AbsolutItemSize : public ItemSize {
        AbsolutItemSize(const u32 height);
    };


    struct RelativeItemSize : public ItemSize {
        RelativeItemSize(const shapes::URect& rect, const double height);
        RelativeItemSize(const Window* window, const double height);
        RelativeItemSize(const Window& window, const double height);
        RelativeItemSize(const Layout& layout, const double height);
    };


    struct ScrollLayout : public FocusLayout {
    private:
        Margin gap;
        Texture m_texture;
        ServiceProvider* m_service_provider;
        shapes::URect main_rect;
        shapes::URect scrollbar_rect;
        shapes::URect scrollbar_mover_rect;
        shapes::URect m_viewport;
        bool is_dragging{ false };
        u32 m_step_size;

    public:
        explicit ScrollLayout(
                ServiceProvider* service_provider,
                u32 focus_id,
                Margin gap,
                std::pair<double, double> margin,
                const Layout& layout,
                bool is_top_level = true
        );

        void render(const ServiceProvider& service_provider) const override;

        helper::BoolWrapper<ui::EventHandleType>
        handle_event(const SDL_Event& event, const Window* window) // NOLINT(readability-function-cognitive-complexity)
                override;

        // see TODO comment below
        [[nodiscard]] Layout get_layout_for_index(u32) override;

        //TODO: with some template paramater and magic make this an option in the base class, so that only get_layout_for_new needs to be overwritten!
        template<typename T, typename... Args>
        void add(ItemSize size, Args... args) {

            const Layout layout = get_layout_for_new(size);

            m_widgets.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)..., layout, false)));
            auto focusable = as_focusable(m_widgets.back().get());
            if (focusable.has_value() and not m_focus_id.has_value()) {
                give_focus(focusable.value());
            }

            const auto total_size = layout.get_rect().bottom_right;

            m_texture = m_service_provider->renderer().get_texture_for_render_target(total_size);
            recalculate_sizes(0);
        }


    private:
        [[nodiscard]] Layout get_layout_for_new(ItemSize size);

        void auto_move_after_focus_change();

        // it's called desired, since it might not be entirely valid
        void recalculate_sizes(i32 desired_scroll_height);
    };

} // namespace ui
