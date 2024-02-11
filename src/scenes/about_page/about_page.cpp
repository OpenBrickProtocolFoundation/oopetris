#include "about_page.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "helper/git_helper.hpp"
#include "manager/resource_manager.hpp"
#include "platform/capabilities.hpp"

#include <fmt/format.h>

namespace scenes {

    AboutPage::AboutPage(ServiceProvider* service_provider, const  ui::Layout& layout) : Scene{service_provider, layout}
, m_main_grid{
    ui::Direction::Vertical,
    ui::RelativeMargin{ layout, ui::Direction::Vertical, 0.05 },
    std::pair<double, double>{ 0.05, 0.05 } ,
    ui::RelativeLayout{layout, 0.0, 0.2, 1.0, 0.5  }
} {

        auto id_helper = ui::IDHelper{};

        m_main_grid.add<ui::Label>(
                id_helper.index(), fmt::format("Git Commit: {}", utils::git_commit()), Color::white(),
                service_provider->fonts().get(FontId::Default), std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        m_main_grid.add<ui::Label>(
                id_helper.index(), fmt::format("Build for: {}", utils::built_for_platform()), Color::white(),
                service_provider->fonts().get(FontId::Default), std::pair<double, double>{ 0.3, 0.6 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        //TODO: add contributors with name, clickable link and image
        /* 
        m_main_grid.add<ui::GridLayout<2>>(
                id_helper.index(), "Return", id_helper.focus_id(), [this](const ui::Button&) { m_should_exit = true; },
                std::pair<double, double>{ 0.15, 0.85 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                std::pair<double, double>{ 0.1, 0.1 }
        );


        m_main_grid.add<ui::GridLayout<2>>(
                id_helper.index(), "Return", id_helper.focus_id(), [this](const ui::Button&) { m_should_exit = true; },
                std::pair<double, double>{ 0.15, 0.85 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                std::pair<double, double>{ 0.1, 0.1 }
        ); */
    }

    [[nodiscard]] Scene::UpdateResult AboutPage::update() {
        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, tl::nullopt };
    }

    void AboutPage::render(const ServiceProvider& service_provider) {

        const auto layout = get_layout();
        service_provider.renderer().draw_rect_filled(layout.get_rect(), Color::black());

        m_main_grid.render(service_provider);
    }

    bool AboutPage::handle_event(const SDL_Event& event, const Window* window) {
        if (m_main_grid.handle_event(event, window)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_should_exit = true;
            return true;
        }

        return false;
    }

} // namespace scenes
