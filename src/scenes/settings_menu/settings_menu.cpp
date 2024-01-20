#include "settings_menu.hpp"
#include "../../constants.hpp"
#include "../../music_manager.hpp"
#include "../../resource_manager.hpp"
#include "../../window.hpp"

namespace scenes {

    SettingsMenu::SettingsMenu(ServiceProvider* service_provider, Window* window)
        : Scene(service_provider),
          m_heading{
              "Settings", Color::white(), service_provider->fonts().get(FontId::Default), ui::AbsoluteLayout{100, 100}
    },
          m_focus_group{ ui::AbsoluteLayout{ 0, 0 } } {

        m_focus_group.add(std::make_unique<ui::Button>(
                "Return", ui::AbsoluteLayout{ 100, 300 }, 100, [this](const ui::Button&) { m_should_exit = true; },
                window
        ));
    }

    [[nodiscard]] Scene::UpdateResult SettingsMenu::update() {
        if (m_should_exit) {
            return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Switch{ SceneId::MainMenu } };
        }
        return UpdateResult{ SceneUpdate::ContinueUpdating, tl::nullopt };
    }

    void SettingsMenu::render(const ServiceProvider& service_provider) {
        m_heading.render(service_provider, service_provider.window().screen_rect());
        m_focus_group.render(service_provider, service_provider.window().screen_rect());
    }

    bool SettingsMenu::handle_event(const SDL_Event& event) {
        return m_focus_group.handle_event(event);
    }

} // namespace scenes
