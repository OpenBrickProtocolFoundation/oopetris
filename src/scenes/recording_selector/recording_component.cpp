
#include "recording_component.hpp"


#include "manager/font.hpp"
#include "manager/resource_manager.hpp"

custom_ui::RecordingComponent::RecordingComponent(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
                const data::RecordingMetadata& metadata,
                const ui::Layout& layout,
                bool is_top_level
        ):ui::Widget{layout, ui::WidgetType::Component, is_top_level},
        ui::Focusable{focus_helper.focus_id()},
        ui::Hoverable{layout.get_rect()},
        m_main_layout{ utils::size_t_identity<2>(), focus_helper.focus_id(), 
        ui::Direction::Vertical,
                    std::array<double, 1>{ 0.6 }, ui::RelativeMargin{layout.get_rect(), ui::Direction::Vertical,0.05}, std::pair<double, double>{ 0.05, 0.03 },
                    layout,false
       },m_metadata{metadata}{

    //TODO: allow empty string in label!

    m_main_layout.add<ui::Label>(
            service_provider, "name: ?", service_provider->fonts().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.3, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    const auto information_layout_index = m_main_layout.add<ui::TileLayout>(
            utils::size_t_identity<3>(), focus_helper.focus_id(), ui::Direction::Horizontal,
            std::array<double, 2>{ 0.33, 0.66 }, ui::AbsolutMargin{ 10 }, std::pair<double, double>{ 0.05, 0.03 }
    );

    const auto information_layout = m_main_layout.get<ui::TileLayout>(information_layout_index);


    information_layout->add<ui::Label>(
            service_provider, "source: ?", service_provider->fonts().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.3, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    information_layout->add<ui::Label>(
            service_provider, "date: ?", service_provider->fonts().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.3, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    information_layout->add<ui::Label>(
            service_provider, "playmode: ?", service_provider->fonts().get(FontId::Default), Color::white(),
            std::pair<double, double>{ 0.3, 0.5 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    populate_texts();
}

void custom_ui::RecordingComponent::render(const ServiceProvider& service_provider) const {

    auto color = has_focus()    ? is_hovered() ? "#00f2ff"_rgb : "#00bbff"_rgb
                 : is_hovered() ? Color(0xC9, 0xC9, 0xC9)
                                : Color(0x9C, 0x9C, 0x9C);

    service_provider.renderer().draw_rect_filled(layout().get_rect(), color);

    m_main_layout.render(service_provider);
}

helper::BoolWrapper<ui::EventHandleType>
custom_ui::RecordingComponent::handle_event(const SDL_Event& event, const Window* window) {

    //TODO
    UNUSED(event);
    UNUSED(window);
    return false;
}


[[nodiscard]] std::tuple<ui::Label*, ui::Label*, ui::Label*, ui::Label*> custom_ui::RecordingComponent::get_texts() {

    auto name_text = m_main_layout.get<ui::Label>(0);

    auto information_layout = m_main_layout.get<ui::TileLayout>(1);

    auto source_text = information_layout->get<ui::Label>(0);
    auto date_text = information_layout->get<ui::Label>(1);
    auto playmode_text = information_layout->get<ui::Label>(2);

    return { name_text, source_text, date_text, playmode_text };
}


void custom_ui::RecordingComponent::populate_texts() {
    //TODO
}
