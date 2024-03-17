
#include "recording_chooser.hpp"


#include "helper/nfd_include.hpp"
#include "manager/font.hpp"
#include "manager/resource_manager.hpp"
#include "ui/components/button.hpp"

custom_ui::RecordingFileChooser::RecordingFileChooser(
                ServiceProvider* service_provider,
                ui::FocusHelper& focus_helper,
                const ui::Layout& layout,
                bool is_top_level
        ):ui::Widget{layout, ui::WidgetType::Component, is_top_level},
        ui::Focusable{focus_helper.focus_id()},
        ui::Hoverable{layout.get_rect()},
        m_main_grid{  focus_helper.focus_id(), 2,
        ui::Direction::Horizontal,
                  ui::RelativeMargin{ layout, ui::Direction::Vertical, 0.05 },
    std::pair<double, double>{ 0.05, 0.05 } ,
    ui::RelativeLayout{layout, 0.0, 0.2, 1.0, 0.5  },false
       }{


    m_main_grid.add<ui::Button>(
            service_provider, "Select Recording", service_provider->fonts().get(FontId::Default), Color::white(),
            focus_helper.focus_id(),
            [](const ui::Button&) {
                //TODO BEFORE PR
                const auto result = helper::openFileDialog({
                        {"OOPetris Recording", { "rec" }}
                });

                result.wait();

                spdlog::info("RESULT: {}", "TODO");
            },
            std::pair<double, double>{ 0.15, 0.85 },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
            std::pair<double, double>{ 0.1, 0.1 }
    );
}

void custom_ui::RecordingFileChooser::render(const ServiceProvider& service_provider) const {
    auto color = has_focus()    ? is_hovered() ? "#00f2ff"_rgb : "#00bbff"_rgb
                 : is_hovered() ? "#C9C9C9"_rgb
                                : "#9C9C9C"_rgb;

    service_provider.renderer().draw_rect_filled(layout().get_rect(), color);

    m_main_grid.render(service_provider);
}

helper::BoolWrapper<ui::EventHandleType>
custom_ui::RecordingFileChooser::handle_event(const SDL_Event& event, const Window* window) {

    //TODO BEFORE PR
    UNUSED(event);
    UNUSED(window);
    return false;
}
