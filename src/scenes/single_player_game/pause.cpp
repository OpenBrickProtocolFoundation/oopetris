#include "pause.hpp"
#include "graphics/renderer.hpp"
#include "helper/platform.hpp"
#include "input/game_input.hpp"
#include "input/input.hpp"
#include "manager/resource_manager.hpp"
#include <fmt/format.h>

namespace scenes {

    SinglePlayerPause::SinglePlayerPause(ServiceProvider* service_provider, const ui::Layout& layout, const std::shared_ptr<input::GameInput>& game_input) : Scene{ service_provider, layout }, m_heading {
        service_provider,
        fmt::format(
            "Pause ({}: continue, {}: quit)",
            game_input->describe_menu_event(input::MenuEvent::PAUSE),
          service_provider->input_manager().get_primary_input()->describe_navigation_event(input::NavigationEvent::BACK)
        ),
        service_provider->font_manager().get(FontId::Default), 
           Color::white(),utils::get_orientation() == utils::Orientation::Landscape
                                       ? std::pair<double, double>{ 0.7, 0.07 }
                                       : std::pair<double, double>{ 0.95, 0.07  },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
               layout,
               true
    },m_game_input{game_input}
    { }

    [[nodiscard]] Scene::UpdateResult scenes::SinglePlayerPause::update() {
        if (m_should_unpause) {
            return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        if (m_should_exit) {
            return UpdateResult{
                SceneUpdate::StopUpdating,
                Scene::Switch{ SceneId::MainMenu, ui::FullScreenLayout{ m_service_provider->window() } }
            };
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void SinglePlayerPause::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black(180));
        m_heading.render(service_provider);
    }

    [[nodiscard]] bool
    SinglePlayerPause::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {

        const auto navigation_event = input_manager->get_navigation_event(event);


        if (m_game_input->get_menu_event(event) == input::MenuEvent::OPEN_SETTINGS) {
            m_should_unpause = true;
            return true;
        }

        if (navigation_event == input::NavigationEvent::BACK) {
            m_should_exit = true;
            return true;
        }

        return false;
    }

} // namespace scenes
