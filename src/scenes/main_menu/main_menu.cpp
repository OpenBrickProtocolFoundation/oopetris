#include "main_menu.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "ui/layout.hpp"

namespace scenes {

    MainMenu::MainMenu(ServiceProvider* service_provider, const  ui::Layout& layout)
        : Scene{service_provider, layout},
          m_main_grid{ ui::Direction::Vertical, ui::RelativeMargin{layout,ui::Direction::Vertical, 0.05}, std::pair<double, double>{ 0.05, 0.05 
            } ,ui::RelativeLayout{ layout, 0.0, 0.2, 1.0, 0.5 }} {

        auto id_helper = ui::IDHelper{};

        m_main_grid.add<ui::Label>(
                id_helper.index(), constants::program_name, Color::white(),
                service_provider->fonts().get(FontId::Default), std::pair<double, double>{ 0.3, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        constexpr auto button_size = utils::device_orientation() == utils::Orientation::Landscape
                                             ? std::pair<double, double>{ 0.15, 0.85 }
                                             : std::pair<double, double>{ 0.5, 0.85 };
        constexpr auto button_alignment =
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center };
        constexpr auto button_margins = utils::device_orientation() == utils::Orientation::Landscape
                                                ? std::pair<double, double>{ 0.1, 0.1 }
                                                : std::pair<double, double>{ 0.2, 0.2 };

        m_main_grid.add<ui::Button>(
                id_helper.index(), "Start", id_helper.focus_id(),
                [this](const ui::Button&) { m_next_command = Command::StartGame; }, button_size, button_alignment,
                button_margins
        );

        m_main_grid.add<ui::Button>(
                id_helper.index(), "Settings", id_helper.focus_id(),
                [this](const ui::Button&) { m_next_command = Command::OpenSettingsMenu; }, button_size,
                button_alignment, button_margins
        );

        m_main_grid.add<ui::Button>(
                id_helper.index(), "About", id_helper.focus_id(),
                [this](const ui::Button&) { m_next_command = Command::OpenAboutPage; }, button_size, button_alignment,
                button_margins
        );

        m_main_grid.add<ui::Button>(
                id_helper.index(), "Exit", id_helper.focus_id(),
                [this](const ui::Button&) { m_next_command = Command::Exit; }, button_size, button_alignment,
                button_margins
        );

        service_provider->music_manager()
                .load_and_play_music(
                        utils::get_assets_folder() / "music" / utils::get_supported_music_extension("01. Main Menu")
                )
                .and_then(utils::log_error);
    }

    [[nodiscard]] Scene::UpdateResult MainMenu::update() {
        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::StartGame:
                    return UpdateResult{
                        SceneUpdate::ContinueUpdating,
                        Scene::Switch{SceneId::Ingame, ui::FullScreenLayout{ m_service_provider->window() }}
                    };
                case Command::OpenAboutPage:
                    // perform a push and reset the command, so that the music keeps playing the entire time
                    m_next_command = tl::nullopt;
                    return UpdateResult{
                        SceneUpdate::ContinueUpdating,
                        Scene::Push{SceneId::AboutPage, ui::FullScreenLayout{ m_service_provider->window() }}
                    };
                case Command::OpenSettingsMenu:
                    // perform a push and reset the command, so that the music keeps playing the entire time
                    m_next_command = tl::nullopt;
                    return UpdateResult{
                        SceneUpdate::ContinueUpdating,
                        Scene::Push{SceneId::SettingsMenu, ui::FullScreenLayout{ m_service_provider->window() }}
                    };
                case Command::Exit:
                    return UpdateResult{ SceneUpdate::ContinueUpdating, Scene::Exit{} };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::ContinueUpdating, tl::nullopt };
    }

    void MainMenu::render(const ServiceProvider& service_provider) {
        m_main_grid.render(service_provider);
    }

    bool MainMenu::handle_event(const SDL_Event& event, const Window* window) {
        if (m_main_grid.handle_event(event, window)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_next_command = Command::Exit;
            return true;
        }
        return false;
    }

} // namespace scenes
