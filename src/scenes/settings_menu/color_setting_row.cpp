
#include <core/helper/errors.hpp>
#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>

#include "color_setting_row.hpp"
#include "input/input.hpp"
#include "ui/components/label.hpp"
#include "ui/focusable.hpp"
#include "ui/hoverable.hpp"
#include "ui/layout.hpp"
#include "ui/widget.hpp"

#include <functional>

detail::ColorSettingRectangle::ColorSettingRectangle(
        const Color& start_color,
        const shapes::URect& fill_rect,
        const ui::Layout& layout,
        bool is_top_level
)
    : Widget{ layout, ui::WidgetType::Component, is_top_level },
      Focusable{ ui::FocusHelper::focus_id_unused() },
      Hoverable{ fill_rect },
      m_color{ start_color },
      m_fill_rect{ fill_rect } { }

detail::ColorSettingRectangle::ColorSettingRectangle(
        const Color& start_color,
        std::pair<double, double> size,
        ui::Alignment alignment,
        const ui::Layout& layout,
        bool is_top_level
)
    : ColorSettingRectangle{ start_color,
                             ui::get_rectangle_aligned(
                                     layout,
                                     { static_cast<u32>(size.first * layout.get_rect().width()),
                                       static_cast<u32>(size.second * layout.get_rect().height()) },
                                     alignment
                             ),
                             layout, is_top_level } { }

[[nodiscard]] Color& detail::ColorSettingRectangle::color() {
    return m_color;
}

[[nodiscard]] const Color& detail::ColorSettingRectangle::color() const {
    return m_color;
}

void detail::ColorSettingRectangle::render(const ServiceProvider& service_provider) const {
    service_provider.renderer().draw_rect_filled(m_fill_rect, m_color);
    //TODO(Totto): maybe use a dynamic color, to have some contrast?
    service_provider.renderer().draw_rect_outline(m_fill_rect, Color::white());
}

ui::Widget::EventHandleResult detail::ColorSettingRectangle::handle_event(
        const std::shared_ptr<input::InputManager>& input_manager,
        const SDL_Event& event
) {

    const auto navigation_event = input_manager->get_navigation_event(event);

    if (has_focus() and navigation_event == input::NavigationEvent::OK) {
        return {
            true,
            { ui::EventHandleType::RequestAction, this, nullptr }
        };
    }


    if (const auto hover_result = detect_hover(input_manager, event); hover_result) {
        if (hover_result.is(ui::ActionType::Clicked)) {
            return {
                true,
                { ui::EventHandleType::RequestAction, this, nullptr }
            };
        }
        return true;
    }

    return false;
}


detail::ColorPickerScene::ColorPickerScene(
        ServiceProvider* service_provider,
        const ui::Layout& layout,
        const Color& starting_color,
        Callback callback
)
    : Scene{
          service_provider, layout
    }, 
    m_color_picker{ service_provider, starting_color, std::move(callback), std::pair<double, double>{ 0.95, 0.95 }, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Bottom }, layout, false } { }

[[nodiscard]] scenes::Scene::UpdateResult detail::ColorPickerScene::update() {
    if (m_should_exit) {
        return UpdateResult{ scenes::SceneUpdate::StopUpdating, Scene::Pop{} };
    }
    return UpdateResult{ scenes::SceneUpdate::StopUpdating, std::nullopt };
}

void detail::ColorPickerScene::render(const ServiceProvider& service_provider) {
    service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black());
    if (not get_layout().is_full_screen()) {
        service_provider.renderer().draw_rect_outline(get_layout().get_rect(), Color::white());
    }

    m_color_picker.render(service_provider);
}
bool detail::ColorPickerScene::handle_event(
        const std::shared_ptr<input::InputManager>& input_manager,
        const SDL_Event& event
) {

    const auto result = m_color_picker.handle_event(input_manager, event);
    if (result) {
        return result;
    }

    const auto navigation_event = input_manager->get_navigation_event(event);


    if (navigation_event == input::NavigationEvent::BACK) {
        m_should_exit = true;
        return true;
    }


    // swallow all events
    return true;
}


custom_ui::ColorSettingRow::ColorSettingRow(
        ServiceProvider* service_provider,
        std::string name,
        const Color& start_color,
        Callback callback,
        u32 focus_id,
        const ui::Layout& layout,
        bool is_top_level
)
    : ui::Widget{ layout, ui::WidgetType::Component, is_top_level },
      ui::Focusable{ focus_id },
      ui::Hoverable{ layout.get_rect() },
      m_service_provider{ service_provider },
      m_main_layout{ utils::SizeIdentity<2>(),
                     ui::FocusHelper::focus_id_unused(),
                     ui::Direction::Horizontal,
                     std::array<double, 1>{ 0.7 },
                     ui::RelativeMargin{ layout.get_rect(), ui::Direction::Vertical, 0.05 },
                     std::pair<double, double>{ 0.05, 0.03 },
                     layout,
                     false },
      m_outer_callback{ std::move(callback) } {


    m_main_layout.add<ui::Label>(
            service_provider, std::move(name), service_provider->font_manager().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.5, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Left, ui::AlignmentVertical::Center }
    );


    m_main_layout.add<detail::ColorSettingRectangle>(
            start_color, std::pair<double, double>{ 0.5, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Right, ui::AlignmentVertical::Center }
    );
}

void custom_ui::ColorSettingRow::render(const ServiceProvider& service_provider) const {
    m_main_layout.render(service_provider);
}

ui::Widget::EventHandleResult custom_ui::ColorSettingRow::handle_event(
        const std::shared_ptr<input::InputManager>& input_manager,
        const SDL_Event& event
) {
    const auto result = m_main_layout.handle_event(input_manager, event);
    if (const auto additional = result.get_additional(); additional.has_value()) {
        if (std::get<0>(additional.value()) == ui::EventHandleType::RequestAction) {
            return {
                result,
                { ui::EventHandleType::RequestAction, this, nullptr }
            };
        }

        throw helper::FatalError(
                fmt::format("Unsupported Handle Type: {}", magic_enum::enum_name(std::get<0>(additional.value())))
        );
    }

    return result;
}

[[nodiscard]] scenes::Scene::Change custom_ui::ColorSettingRow::get_details_scene() {
    return scenes::Scene::RawPush{
        "ColorPicker",
        std::make_unique<detail::ColorPickerScene>(
                m_service_provider, ui::RelativeLayout{ m_service_provider->window(), 0.1, 0.2, 0.8, 0.6 },
                color_rect()->color(), [this](const Color& color) { this->inner_callback(color); }
        )
    };
}

void custom_ui::ColorSettingRow::inner_callback(const Color& color) {
    color_rect()->color() = color;
    m_outer_callback(color);
}

[[nodiscard]] detail::ColorSettingRectangle* custom_ui::ColorSettingRow::color_rect() {
    return m_main_layout.get<detail::ColorSettingRectangle>(1);
}

[[nodiscard]] const detail::ColorSettingRectangle* custom_ui::ColorSettingRow::color_rect() const {
    return m_main_layout.get<detail::ColorSettingRectangle>(1);
}


void custom_ui::ColorSettingRow::on_focus() {
    if (not m_main_layout.has_focus()) {
        m_main_layout.focus();
    }

    if (not color_rect()->has_focus()) {
        color_rect()->focus();
    }
}

void custom_ui::ColorSettingRow::on_unfocus() {
    if (m_main_layout.has_focus()) {
        m_main_layout.unfocus();
    }

    if (color_rect()->has_focus()) {
        color_rect()->unfocus();
    }
}
