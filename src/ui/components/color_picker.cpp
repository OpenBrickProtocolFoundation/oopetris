

#include "color_picker.hpp"
#include "graphics/point.hpp"
#include "graphics/rect.hpp"
#include "helper/color.hpp"
#include "helper/graphic_utils.hpp"
#include "helper/utils.hpp"
#include "input/input.hpp"
#include "manager/resource_manager.hpp"
#include "ui/components/textinput.hpp"
#include "ui/layout.hpp"
#include "ui/widget.hpp"

#include <memory>

detail::ColorSlider::ColorSlider(
        ServiceProvider* service_provider,
        Range range,
        Getter getter,
        Setter setter,
        double step,
        const ui::Layout& layout,
        bool is_top_level
)
    : AbstractSlider<double>{ ui::FocusHelper::focus_id_unused(),
                              std::move(range),
                              std::move(getter),
                              std::move(setter),
                              step,
                              layout,
                              is_top_level } {

    change_layout();

    m_texture = std::make_unique<Texture>(
            service_provider->renderer().get_texture_for_render_target(bar_rect().to_dimension_point())
    );

    service_provider->renderer().set_render_target(*m_texture);

    const auto w = bar_rect().width();
    const auto h = bar_rect().height();

    for (u32 x = 0; x < w; x++) {
        const Color color =
                HSVColor((static_cast<double>(x) / static_cast<double>(w)) * 360.0, 1.0, 1.0).to_rgb_color();

        service_provider->renderer().draw_line(
                shapes::UPoint{ x, 0 }, shapes::UPoint{ x, static_cast<u32>(h - 1) }, color
        );
    }

    service_provider->renderer().reset_render_target();
}

[[nodiscard]] std::pair<shapes::URect, shapes::URect> detail::ColorSlider::get_rectangles() const {

    const auto layout_rect = layout().get_rect();

    const auto origin = layout_rect.top_left;

    const double percentage = (current_value() - range().first) / (range().second - range().first);

    const u32 position_x_middle =
            origin.x + static_cast<u32>(percentage * static_cast<double>(layout_rect.bottom_right.x - origin.x));

    constexpr const u32 slider_half_size = 2;

    u32 slider_start_x = 0;
    u32 slider_end_x = 0;

    if (position_x_middle <= origin.x) {
        slider_start_x = origin.x;
        slider_end_x = slider_start_x + 2 * slider_half_size;
    } else if (position_x_middle >= layout_rect.bottom_right.x) {
        slider_end_x = layout_rect.bottom_right.x;
        slider_start_x = slider_end_x - 2 * slider_half_size;
    } else {
        slider_start_x = position_x_middle - slider_half_size;
        slider_end_x = position_x_middle + slider_half_size;
    }

    const auto slider_rect = shapes::URect{
        shapes::UPoint{ slider_start_x,     layout_rect.top_left.y },
        shapes::UPoint{   slider_end_x, layout_rect.bottom_right.y }
    };

    return { layout_rect, slider_rect };
}

void detail::ColorSlider::render(const ServiceProvider& service_provider) const {

    const auto& renderer = service_provider.renderer();

    renderer.draw_texture(*m_texture, bar_rect());

    renderer.draw_rect_filled(slider_rect(), Color::white(0xAA));
}


detail::ColorCanvas::ColorCanvas(
        ServiceProvider* service_provider,
        const Color& start_color,
        Callback callback,
        const ui::Layout& layout,
        bool is_top_level
)
    : ui::Widget{ layout, ui::WidgetType::Component, is_top_level },
      m_service_provider{ service_provider },
      m_current_color{ start_color.to_hsv_color() },
      m_callback{ std::move(callback) } {
    redraw_texture();
}


detail::ColorCanvas::~ColorCanvas() {
    SDL_CaptureMouse(SDL_FALSE);
}

void detail::ColorCanvas::render(const ServiceProvider& service_provider) const {
    const auto& renderer = service_provider.renderer();

    renderer.draw_texture(*m_texture, layout().get_rect());

    draw_pseudo_circle(service_provider);
}

void detail::ColorCanvas::draw_pseudo_circle(const ServiceProvider& service_provider) const {
    const auto& renderer = service_provider.renderer();

    u32 diameter = 5;

    // width == height here, since we assured that in the construction of the layout fro this component, so instead of taking the min of width and height i just use width, this fact is only used here, since it's not that bad, if it changes sometime in the future
    if (const double percentage_diameter = static_cast<double>(layout().get_rect().width()) * 0.03;
        percentage_diameter >= static_cast<double>(diameter)) {
        diameter = static_cast<u32>(percentage_diameter);
    }

    const auto [width, height] = layout().get_rect().to_dimension_point().cast<double>();

    const auto center =
            shapes::AbstractPoint<double>(m_current_color.s * width, (1.0 - m_current_color.v) * height).cast<u32>()
            + layout().get_rect().top_left;

    const auto circle_color = m_is_dragging ? "#C7C7C7"_c : "#FFFFFF"_c;

    renderer.draw_self_computed_circle(center, diameter, circle_color);
}

helper::BoolWrapper<std::pair<ui::EventHandleType, ui::Widget*>>
detail::ColorCanvas::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {
    Widget::EventHandleResult handled = false;

    const auto fill_rect = layout().get_rect();

    const auto pointer_event = input_manager->get_pointer_event(event);

    if (pointer_event == input::PointerEvent::PointerDown) {

        if (pointer_event->is_in(fill_rect)) {

            m_is_dragging = true;
            SDL_CaptureMouse(SDL_TRUE);
            handled = {
                true,
                { ui::EventHandleType::RequestFocus, this }
            };
        }
    } else if (pointer_event == input::PointerEvent::PointerUp) {
        // only handle this, if already dragging, otherwise it's a button down from previously or some other widget
        if (m_is_dragging) {
            m_is_dragging = false;
            SDL_CaptureMouse(SDL_FALSE);
            handled = true;
        }
    } else if (pointer_event == input::PointerEvent::Motion) {

        if (m_is_dragging) {
            handled = true;
        }
    }


    if (handled and pointer_event.has_value()) {

        const auto previous_color = m_current_color;

        const auto& [x, y] = pointer_event.value().position();

        if (x <= static_cast<i32>(fill_rect.top_left.x)) {
            m_current_color.s = 0.0;
        } else if (x >= static_cast<i32>(fill_rect.bottom_right.x)) {
            m_current_color.s = 1.0;
        } else {
            const double percentage =
                    static_cast<double>(x - fill_rect.top_left.x) / static_cast<double>(fill_rect.width());
            m_current_color.s = percentage;
        }

        if (y <= static_cast<i32>(fill_rect.top_left.y)) {
            m_current_color.v = 1.0;
        } else if (y >= static_cast<i32>(fill_rect.bottom_right.y)) {
            m_current_color.v = 0.0;
        } else {
            const double percentage =
                    1.0 - static_cast<double>(y - fill_rect.top_left.y) / static_cast<double>(fill_rect.height());
            m_current_color.v = percentage;
        }

        // if we hover at e.g. the edges, and don't change anything, we don't need to call the callback
        if (previous_color.s == m_current_color.s && previous_color.v == m_current_color.v) {
            return handled;
        }

        const shapes::AbstractPoint<double> point = { m_current_color.s, m_current_color.v };
        m_callback(point);

        return handled;
    }

    return false;


    return false;
}

void detail::ColorCanvas::on_change(ColorChangeOrigin origin, const HSVColor& color) {
    switch (origin) {
        case detail::ColorChangeOrigin::TextInput: {
            m_current_color = color;
            break;
        }
        case detail::ColorChangeOrigin::Canvas: {
            //nothing to do, shouldn't be reached
            UNREACHABLE();
            break;
        }
        case detail::ColorChangeOrigin::Slider: {
            // only need to change the h value
            m_current_color.h = color.h;
            break;
        }
        default:
            UNREACHABLE();
    }

    redraw_texture();
}

void detail::ColorCanvas::redraw_texture() {

    const auto fill_rect = layout().get_rect();

    m_texture = std::make_unique<Texture>(
            m_service_provider->renderer().get_texture_for_render_target(fill_rect.to_dimension_point())
    );

    m_service_provider->renderer().set_render_target(*m_texture);

    const auto width = fill_rect.width();
    const auto height = fill_rect.height();

    const auto hue = m_current_color.h;

    //TODO(Totto): try to speed this up, since it is a performance bottle neck, if hovering like a madman (xD) over the color slider
    // maybe use shaders?
    for (u32 y = 0; y < height; y++) {
        const auto value = 1.0 - (static_cast<double>(y) / static_cast<double>(height));
        for (u32 x = 0; x < width; x++) {
            const Color color =
                    HSVColor(hue, static_cast<double>(x) / static_cast<double>(width), value).to_rgb_color();

            m_service_provider->renderer().draw_pixel(shapes::UPoint{ x, y }, color);
        }
    }

    m_service_provider->renderer().reset_render_target();
}


ui::ColorPicker::ColorPicker(
        ServiceProvider* service_provider,
        const Color& start_color,
        Callback callback,
        const shapes::URect& fill_rect,
        const Layout& layout,
        bool is_top_level
)
    : Widget{ layout, WidgetType::Component, is_top_level },
      m_color{ start_color },
      m_mode{ ColorMode::RGB },
      m_callback{ std::move(callback) } {

    //TODO(Totto): add alpha slider at the side

    constexpr double main_rect_height = 0.8;

    const auto main_fill_layout = ui::RelativeLayout{ fill_rect, 0.0, 0.0, 1.0, main_rect_height };

    const auto main_rect = ui::get_rectangle_aligned(
            main_fill_layout, ui::ratio_helper(main_fill_layout.get_rect().to_dimension_pair(), true, { 512, 512 }),
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Bottom }
    );

    m_color_canvas = std::make_unique<detail::ColorCanvas>(
            service_provider, start_color,
            [this](const shapes::AbstractPoint<double>& value) {
                auto hsv_color = m_color.to_hsv_color();
                hsv_color.s = value.x;
                hsv_color.v = value.y;
                this->m_color = Color{ hsv_color };
                this->after_color_change(detail::ColorChangeOrigin::Canvas, hsv_color);
            },
            ui::Layout{ main_rect }, false
    );

    auto rest_fill_rect = ui::RelativeLayout(fill_rect, 0.0, main_rect_height, 1.0, 1.0 - main_rect_height).get_rect();

    // align it to the horizontal size of the main_rect
    rest_fill_rect.top_left.x = main_rect.top_left.x;
    rest_fill_rect.bottom_right.x = main_rect.bottom_right.x;

    constexpr double preview_rect_size = 0.2;

    const auto preview_fill_layout =
            ui::RelativeLayout{ rest_fill_rect, 1.0 - preview_rect_size, 0.0, preview_rect_size, 1.0 };

    m_color_preview = ui::get_rectangle_aligned(
            preview_fill_layout,
            ui::ratio_helper(preview_fill_layout.get_rect().to_dimension_pair(), true, { 512, 512 }),
            ui::Alignment{ ui::AlignmentHorizontal::Left, ui::AlignmentVertical::Top }
    );


    const auto bars_fill_rect = ui::RelativeLayout(rest_fill_rect, 0.0, 0.0, 1.0 - preview_rect_size, 1.0).get_rect();

    constexpr double color_bar_size = 0.5;

    const auto color_bar_fill_layout = ui::RelativeLayout{ bars_fill_rect, 0.0, 0.0, 1.0, color_bar_size };

    const auto color_bar_rect = ui::get_rectangle_aligned(
            color_bar_fill_layout, color_bar_fill_layout.get_rect().to_dimension_pair(),
            ui::Alignment{ ui::AlignmentHorizontal::Right, ui::AlignmentVertical::Top }
    );

    m_color_slider = std::make_unique<detail::ColorSlider>(
            service_provider, std::pair<double, double>{ 0.0, 360.0 },
            [this]() -> double { return this->m_color.to_hsv_color().h; },
            [this](double value) {
                auto hsv_color = m_color.to_hsv_color();
                hsv_color.h = value;

                const auto previous_color = this->m_color;

                this->m_color = Color{ hsv_color };

                // if we hover at e.g. the edges, and don't change anything, we don't need to call after_color_change
                // this also helps, since the slider reports change on click and on release, even if the position doesn't change there
                if (previous_color == this->m_color) {
                    return;
                }

                this->after_color_change(detail::ColorChangeOrigin::Slider, hsv_color);
            },
            5.0, ui::Layout{ color_bar_rect }, false
    );

    const auto components_fill_layout =
            ui::RelativeLayout{ bars_fill_rect, 0.0, color_bar_size, 1.0, 1.0 - color_bar_size };

    constexpr double toggle_button_size = 0.2;

    const auto toggle_button_layout = ui::RelativeLayout{ components_fill_layout, 0.0, 0.0, toggle_button_size, 1.0 };


    const auto focus_id_unused = FocusHelper::focus_id_unused();

    const auto rgb_image_path = utils::get_assets_folder() / "images" / "rgb_color_selector.png";

    m_rgb_button = std::make_unique<ui::ImageButton>(
            service_provider, rgb_image_path, true, focus_id_unused,
            [this](const ImageButton&) -> bool {
                this->m_mode = ColorMode::HSV;
                this->change_text();
                return false;
            },
            std::pair<double, double>{ 0.95, 0.95 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }, toggle_button_layout, false
    );

    const auto hsv_image_path = utils::get_assets_folder() / "images" / "hsv_color_selector.png";

    m_hsv_button = std::make_unique<ui::ImageButton>(
            service_provider, hsv_image_path, true, focus_id_unused,
            [this](const ImageButton&) -> bool {
                this->m_mode = ColorMode::RGB;
                this->change_text();
                return false;
            },
            std::pair<double, double>{ 0.95, 0.95 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }, toggle_button_layout, false
    );

    const auto textinput_layout =
            ui::RelativeLayout{ components_fill_layout, toggle_button_size, 0.0, 1.0 - toggle_button_size, 1.0 };


    m_color_text = std::make_unique<ui::TextInput>(
            service_provider, service_provider->font_manager().get(FontId::Default), Color::white(), focus_id_unused,
            std::pair<double, double>{ 0.9, 0.9 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }, ui::TextInputMode::Scale,
            textinput_layout, false
    );

    // using SLider, that behaviour simulates the one, that I want, since the ColorSlider already gets the change by the getter,
    // but the textinput and the canvas need to be set manually by this call
    after_color_change(detail::ColorChangeOrigin::Slider, m_color.to_hsv_color());
}

ui::ColorPicker::ColorPicker(
        ServiceProvider* service_provider,
        const Color& start_color,
        Callback callback,
        std::pair<double, double> size,
        Alignment alignment,
        const Layout& layout,
        bool is_top_level
)
    : ColorPicker{ service_provider,
                   start_color,
                   std::move(callback),
                   ui::get_rectangle_aligned(
                           layout,
                           { static_cast<u32>(size.first * layout.get_rect().width()),
                             static_cast<u32>(size.second * layout.get_rect().height()) },
                           alignment
                   ),
                   layout,
                   is_top_level } { }

void ui::ColorPicker::render(const ServiceProvider& service_provider) const {
    const auto& renderer = service_provider.renderer();

    m_color_canvas->render(service_provider);

    renderer.draw_rect_filled(m_color_preview, m_color);

    m_color_slider->render(service_provider);

    if (m_mode == ColorMode::HSV) {
        m_hsv_button->render(service_provider);
    } else {
        m_rgb_button->render(service_provider);
    }

    m_color_text->render(service_provider);
}

helper::BoolWrapper<std::pair<ui::EventHandleType, ui::Widget*>>
ui::ColorPicker::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {

    auto handled = m_color_slider->handle_event(input_manager, event);

    if (handled) {
        return handled;
    }

    handled = m_color_canvas->handle_event(input_manager, event);

    if (handled) {
        return handled;
    }


    if (m_mode == ColorMode::HSV) {
        handled = m_hsv_button->handle_event(input_manager, event);
    } else {
        handled = m_rgb_button->handle_event(input_manager, event);
    }

    if (handled) {
        return handled;
    }

    handled = m_color_text->handle_event(input_manager, event);

    if (handled) {
        if (const auto additional = handled.get_additional(); additional.has_value()) {
            switch (additional.value().first) {
                case ui::EventHandleType::RequestFocus:
                    if (not m_color_text->has_focus()) {
                        m_color_text->focus();
                    }
                    break;
                case ui::EventHandleType::RequestUnFocus:
                    if (m_color_text->has_focus()) {
                        m_color_text->unfocus();
                    }
                    break;
                case ui::EventHandleType::RequestAction: {
                    const auto maybe_color = HSVColor::from_string(m_color_text->get_text());

                    if (m_color_text->has_focus()) {
                        m_color_text->unfocus();
                    }

                    if (not maybe_color.has_value()) {
                        //TODO(Totto): maybe inform the user, that the input is incorrect?
                        //m_color_text->display_error();

                        // reset the text
                        change_text();
                        break;
                    }

                    const auto color = maybe_color.value();
                    m_color = Color{ color };

                    after_color_change(detail::ColorChangeOrigin::TextInput, color);

                    break;
                }
                default:
                    UNREACHABLE();
            }
        }

        return handled;
    }

    return false;
}

void ui::ColorPicker::after_color_change(detail::ColorChangeOrigin origin, const HSVColor& color) {
    switch (origin) {
        case detail::ColorChangeOrigin::TextInput: {
            m_color_slider->on_change();

            m_color_canvas->on_change(origin, color);
            break;
        }
        case detail::ColorChangeOrigin::Canvas: {
            change_text();
            break;
        }
        case detail::ColorChangeOrigin::Slider: {
            m_color_canvas->on_change(origin, color);

            change_text();
            break;
        }
        default:
            UNREACHABLE();
    }

    m_callback(m_color);
}

void ui::ColorPicker::change_text() {
    const std::string text =
            m_color.to_string(m_mode == ColorMode::HSV ? color::SerializeMode::HSV : color::SerializeMode::RGB, false);

    m_color_text->set_text(text);
}
