#include <core/helper/date.hpp>
#include <core/helper/magic_enum_wrapper.hpp>

#include "input/input.hpp"
#include "manager/font.hpp"
#include "manager/resource_manager.hpp"
#include "recording_component.hpp"
#include "ui/components/text_button.hpp"
#include "ui/widget.hpp"

#include <fmt/format.h>

#if defined(_ENABLE_REPLAY_RENDERING)
#include "graphics/video_renderer.hpp"
#endif

custom_ui::RecordingComponent::RecordingComponent(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
                data::RecordingMetadata metadata,
                const ui::Layout& layout,
                bool is_top_level
        ):ui::Widget{layout, ui::WidgetType::Component, is_top_level},
        ui::Focusable{focus_helper.focus_id()},
        ui::Hoverable{layout.get_rect()},
        m_main_layout{ utils::SizeIdentity<2>(), focus_helper.focus_id(), 
        ui::Direction::Horizontal,
                    std::array<double, 1>{ 0.9 }, ui::RelativeMargin{layout.get_rect(), ui::Direction::Vertical,0.05}, std::pair<double, double>{ 0.05, 0.03 },
                    layout,false
       },m_metadata{std::move(metadata)},m_current_focus_id{m_main_layout.focus_id()}{


    auto text_layout_index = m_main_layout.add<ui::TileLayout>(
            utils::SizeIdentity<2>(), focus_helper.focus_id(), ui::Direction::Vertical, std::array<double, 1>{ 0.6 },
            ui::RelativeMargin{ layout.get_rect(), ui::Direction::Vertical, 0.05 },
            std::pair<double, double>{ 0.05, 0.03 }
    );

    auto* text_layout = m_main_layout.get<ui::TileLayout>(text_layout_index);


    auto render_button_index = m_main_layout.add<ui::TextButton>(
            service_provider, "Render", service_provider->font_manager().get(FontId::Default), Color::white(),
            focus_helper.focus_id(), [](const ui::TextButton&) -> bool { return false; },
            std::pair<double, double>{ 0.95, 0.85 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
            std::pair<double, double>{ 0.1, 0.1 }
    );

    auto* render_button = m_main_layout.get<ui::TextButton>(render_button_index);

    render_button->disable();

#if defined(_ENABLE_REPLAY_RENDERING)
    VideoRendererBackend::is_supported_async([render_button](bool is_supported) {
        if (is_supported) {
            render_button->enable();
        }
    });
#endif

    text_layout->add<ui::Label>(
            service_provider, "name: ?", service_provider->font_manager().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.5, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );


    const auto information_layout_index = text_layout->add<ui::TileLayout>(
            utils::SizeIdentity<3>(), focus_helper.focus_id(), ui::Direction::Horizontal,
            std::array<double, 2>{ 0.33, 0.66 }, ui::AbsolutMargin{ 10 }, std::pair<double, double>{ 0.05, 0.03 }
    );

    auto* const information_layout = text_layout->get<ui::TileLayout>(information_layout_index);


    information_layout->add<ui::Label>(
            service_provider, "source: ?", service_provider->font_manager().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.9, 0.9 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    information_layout->add<ui::Label>(
            service_provider, "date: ?", service_provider->font_manager().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.9, 0.9 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    information_layout->add<ui::Label>(
            service_provider, "playmode: ?", service_provider->font_manager().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.9, 0.9 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    populate_texts(service_provider);
}

void custom_ui::RecordingComponent::render(const ServiceProvider& service_provider) const {

    const auto color = has_focus()    ? is_hovered() ? "#00f2ff"_c : "#00bbff"_c
                       : is_hovered() ? "hsv(0, 0, 0.79)"_c
                                      : "#9C9C9C"_c;

    service_provider.renderer().draw_rect_filled(layout().get_rect(), color);

    m_main_layout.render(service_provider);
}

ui::Widget::EventHandleResult custom_ui::RecordingComponent::handle_event(
        const std::shared_ptr<input::InputManager>& input_manager,
        const SDL_Event& event
) {

    auto* render_button = m_main_layout.get<ui::TextButton>(1);

    if (has_focus() and input_manager->get_navigation_event(event) == input::NavigationEvent::OK) {
        if (m_current_focus_id == m_main_layout.focus_id()) {
            return {
                true,
                { .handle_type = ui::EventHandleType::RequestAction, .widget = this, .data = nullptr }
            };
        }

        if (m_current_focus_id == render_button->focus_id()) {
            return {
                true,
                { .handle_type = ui::EventHandleType::RequestAction, .widget = render_button, .data = nullptr }
            };
        }

        spdlog::error("Recording selector has invalid focused element: {}", m_current_focus_id);
    }

    if (has_focus()
        and (input_manager->get_navigation_event(event) == input::NavigationEvent::LEFT
             or input_manager->get_navigation_event(event) == input::NavigationEvent::RIGHT)) {

        if (m_current_focus_id == m_main_layout.focus_id()) {
            m_current_focus_id = render_button->focus_id();
            return true;
        }

        if (m_current_focus_id == render_button->focus_id()) {
            m_current_focus_id = m_main_layout.focus_id();
            return true;
        }

        spdlog::error("Recording selector has invalid focused element: {}", m_current_focus_id);
    }


    if (const auto hover_result = detect_hover(input_manager, event); hover_result) {


        if (const auto render_button_hover_result = render_button->detect_hover(input_manager, event);
            render_button_hover_result) {

            this->on_unhover();

            if (render_button_hover_result.is(ui::ActionType::Clicked)) {

                if (not has_focus()) {
                    return {
                        true,
                        { .handle_type = ui::EventHandleType::RequestFocus, .widget = this, .data = nullptr }
                    };
                }

                return {
                    true,
                    { .handle_type = ui::EventHandleType::RequestAction, .widget = render_button, .data = this }
                };
            }

            return true;
        }

        if (hover_result.is(ui::ActionType::Clicked)) {

            return {
                true,
                { .handle_type = has_focus() ? ui::EventHandleType::RequestAction : ui::EventHandleType::RequestFocus,
                  .widget = this,
                  .data = nullptr }
            };
        }
        return true;
    }

    return false;
}


[[nodiscard]] data::RecordingMetadata custom_ui::RecordingComponent::metadata() const {
    return m_metadata;
}

[[nodiscard]] std::tuple<ui::Label*, ui::Label*, ui::Label*, ui::Label*> custom_ui::RecordingComponent::get_texts() {

    auto* text_layout = m_main_layout.get<ui::TileLayout>(0);

    auto* name_text = text_layout->get<ui::Label>(0);

    auto* information_layout = text_layout->get<ui::TileLayout>(1);

    auto* source_text = information_layout->get<ui::Label>(0);
    auto* date_text = information_layout->get<ui::Label>(1);
    auto* playmode_text = information_layout->get<ui::Label>(2);

    return { name_text, source_text, date_text, playmode_text };
}


void custom_ui::RecordingComponent::populate_texts(ServiceProvider* service_provider) {

    auto [name_text, source_text, date_text, playmode_text] = get_texts();

    const auto& info = m_metadata.information;

    std::string name_text_value{};
    if (const auto name_info = info.get_if<std::string>("name"); name_info.has_value()) {
        name_text_value = name_info.value();
    } else {
        name_text_value = m_metadata.path.filename().string();
    }
    name_text->set_text(*service_provider, fmt::format("name: {}", name_text_value));

    source_text->set_text(*service_provider, fmt::format("source: {}", magic_enum::enum_name(m_metadata.source)));


    std::string date_text_value = "unknown";
    if (const auto date_info_string = info.get_if<std::string>("date"); date_info_string.has_value()) {
        date_text_value = date_info_string.value();
    } else if (const auto date_info_number = info.get_if<u64>("date"); date_info_number.has_value()) {
        const auto formatted_date_value = date::ISO8601Date(date_info_number.value()).to_string();
        if (formatted_date_value.has_value()) {
            date_text_value = formatted_date_value.value();
        }
    } else {
        if (const auto date_iso8601_value = date::ISO8601Date::from_string(m_metadata.path.stem().string());
            date_iso8601_value.has_value()) {
            const auto formatted_date_value = date_iso8601_value.value().to_string();
            if (formatted_date_value.has_value()) {
                date_text_value = formatted_date_value.value();
            }
        }
    }
    date_text->set_text(*service_provider, fmt::format("date: {}", date_text_value));

    std::string playmode_text_value = "unknown";
    if (const auto mode_info = info.get_if<std::string>("mode"); mode_info.has_value()) {
        playmode_text_value = mode_info.value();
    } else if (const auto playmode_info = info.get_if<std::string>("playmode"); playmode_info.has_value()) {
        playmode_text_value = playmode_info.value();
    }
    playmode_text->set_text(*service_provider, fmt::format("playmode: {}", playmode_text_value));
}
