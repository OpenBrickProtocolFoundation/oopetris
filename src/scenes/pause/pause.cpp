#include "pause.hpp"
#include "../../capabilities.hpp"
#include "../../renderer.hpp"
#include "../../resource_manager.hpp"


#if defined(__SWITCH__)
#include "../../switch_buttons.hpp"
#endif
namespace scenes {


    Pause::Pause(ServiceProvider* service_provider) : Scene{ service_provider }, m_heading {
      "Pause (" + 
        utils::action_description(utils::CrossPlatformAction::UNPAUSE) + ": continue, "+utils::action_description(utils::CrossPlatformAction::EXIT)+": quit)"
                ,
                Color::white(), service_provider->fonts().get(FontId::Default), ui::AbsoluteLayout {
            50, 50
        }
    }
    { }

    [[nodiscard]] Scene::UpdateResult scenes::Pause::update() {
        if (m_should_unpause) {
            return std::pair{ SceneUpdate::StopUpdating, Scene::Pop{} };
        }
        if (m_should_exit) {
            return std::pair{ SceneUpdate::StopUpdating, Scene::Switch{ SceneId::MainMenu } };
        }
        return std::pair{ SceneUpdate::StopUpdating, tl::nullopt };
    }

    void Pause::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(service_provider.window().screen_rect(), Color::black(180));
        m_heading.render(service_provider, service_provider.window().screen_rect());
    }

    [[nodiscard]] bool Pause::handle_event(const SDL_Event& event) {

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
