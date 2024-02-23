#include "pause.hpp"
#include "graphics/renderer.hpp"
#include "manager/resource_manager.hpp"
#include "platform/capabilities.hpp"
#include <fmt/format.h>


#if defined(__SWITCH__)
#include "platform/switch_buttons.hpp"
#endif
namespace scenes {

    Pause::Pause(ServiceProvider* service_provider, const ui::Layout& layout) : Scene{ service_provider, layout }, m_heading {
        service_provider,
        fmt::format(
            "Pause ({}: continue, {}: quit)",
            utils::action_description(utils::CrossPlatformAction::UNPAUSE),
            utils::action_description(utils::CrossPlatformAction::EXIT)
        ),
        service_provider->fonts().get(FontId::Default), 
           Color::white(),utils::device_orientation() == utils::Orientation::Landscape
                                       ? std::pair<double, double>{ 0.7, 0.07 }
                                       : std::pair<double, double>{ 0.95, 0.07  },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
               layout
    }
    { }

    [[nodiscard]] Scene::UpdateResult scenes::Pause::update() {
        if (m_should_unpause) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        if (m_should_exit) {
            return UpdateResult{
                SceneUpdate::StopUpdating,
                Scene::Switch{SceneId::MainMenu, ui::FullScreenLayout{ m_service_provider->window() }}
            };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void Pause::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black(180));
        m_heading.render(service_provider);
    }

    [[nodiscard]] bool Pause::handle_event(const SDL_Event& event, const Window*) {

        if (utils::event_is_action(event, utils::CrossPlatformAction::UNPAUSE)) {
            m_should_unpause = true;
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::EXIT)) {
            m_should_exit = true;
            return true;
        }

        return false;
    }

} // namespace scenes
