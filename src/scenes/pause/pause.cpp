#include "pause.hpp"
#include "../../renderer.hpp"
#include "../../resource_manager.hpp"

#if defined(__SWITCH__)
#include "../../switch_buttons.hpp"
#endif
namespace scenes {


    Pause::Pause(ServiceProvider* service_provider) : Scene{ service_provider }, m_heading {
#if defined(__ANDROID__)
        "Pause (Tap anywhere: continue, Back Button: quit)"
#elif defined(__SWITCH__)
        "Pause (PLUS: continue, MINUS: quit)"
#else
        "Pause (ESC: continue, Return: quit)"
#endif
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

#if defined(__ANDROID__)
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_AC_BACK) {
            m_should_exit = true;
            return true;
        } else if (event.type == SDL_FINGERDOWN) {
            // if we tap anywhere outside of the apps focus (eg title bar, back key, etc. ) it in't registered, any other press resumes (unpauses) the game
            m_should_unpause = true;
            return true;
        }
#elif defined(__SWITCH__)
        if (event.type == SDL_JOYBUTTONDOWN) {
            switch (event.jbutton.button) {
                case JOYCON_PLUS:
                    m_should_unpause = true;
                    return true;
                case JOYCON_MINUS:
                    m_should_exit = true;
                    return true;
            }
        }
#else
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    m_should_unpause = true;
                    return true;
                case SDLK_RETURN:
                    m_should_exit = true;
                    return true;
            }
        }
#endif

        return false;
    }

} // namespace scenes
