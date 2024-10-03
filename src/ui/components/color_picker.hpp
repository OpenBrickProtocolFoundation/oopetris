#pragma once


#include <core/helper/color.hpp>
#include <core/helper/point.hpp>

#include "graphics/rect.hpp"
#include "ui/components/abstract_slider.hpp"
#include "ui/components/image_button.hpp"
#include "ui/components/textinput.hpp"
#include "ui/widget.hpp"

#include <memory>

namespace detail {

    // it is intended, that this never has focus, than the scroll wheel doesn't work, but it shouldn't work, since scrolling a color slider isn't intended behaviour
    struct ColorSlider : public ui::AbstractSlider<double> {
    private:
        std::unique_ptr<Texture> m_texture;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ColorSlider(
                ServiceProvider* service_provider,
                Range range,
                Getter getter,
                Setter setter,
                double step,
                const ui::Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

    private:
        [[nodiscard]] std::pair<shapes::URect, shapes::URect> get_rectangles() const override;
    };


    enum class ColorChangeOrigin : u8 { TextInput, Canvas, Slider };

    struct ColorCanvas : public ui::Widget {
    public:
        using Callback = std::function<void(const shapes::AbstractPoint<double>& value)>;

    private:
        ServiceProvider* m_service_provider;
        std::unique_ptr<Texture> m_texture{};
        HSVColor m_current_color;
        Callback m_callback;
        bool m_is_dragging{ false };

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ColorCanvas(
                ServiceProvider* service_provider,
                const Color& start_color,
                Callback callback,
                const ui::Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED ~ColorCanvas() override;

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED void on_change(ColorChangeOrigin origin, const HSVColor& color);

        OOPETRIS_GRAPHICS_EXPORTED void draw_pseudo_circle(const ServiceProvider& service_provider) const;

    private:
        void redraw_texture();
    };


} // namespace detail


namespace ui {

    enum class ColorMode : u8 { RGB, HSV };

    struct ColorPicker final : public Widget {
        using Callback = std::function<void(const Color&)>;

    private:
        Color m_color;
        std::unique_ptr<detail::ColorCanvas> m_color_canvas;
        shapes::URect m_color_preview;
        std::unique_ptr<detail::ColorSlider> m_color_slider;
        ColorMode m_mode;
        Callback m_callback;
        std::unique_ptr<ui::ImageButton> m_rgb_button;
        std::unique_ptr<ui::ImageButton> m_hsv_button;
        std::unique_ptr<ui::TextInput> m_color_text;

        explicit ColorPicker(
                ServiceProvider* service_provider,
                const Color& start_color,
                Callback callback,
                const shapes::URect& fill_rect,
                const Layout& layout,
                bool is_top_level
        );

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ColorPicker(
                ServiceProvider* service_provider,
                const Color& start_color,
                Callback callback,
                std::pair<double, double> size,
                Alignment alignment,
                const Layout& layout,
                bool is_top_level
        );

        OOPETRIS_GRAPHICS_EXPORTED void render(const ServiceProvider& service_provider) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Widget::EventHandleResult
        handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] Color get_color() const;

    private:
        void after_color_change(detail::ColorChangeOrigin origin, const HSVColor& color);
        void change_text();
    };

} // namespace ui
