#include "multiplayer_menu.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "helper/platform.hpp"
#include "input/input.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "ui/layout.hpp"

namespace scenes {

    MultiPlayerMenu::MultiPlayerMenu(ServiceProvider* service_provider, const  ui::Layout& layout)
        : Scene{service_provider, layout},
          m_main_grid{0,5, ui::Direction::Vertical, ui::RelativeMargin{layout,ui::Direction::Vertical, 0.05}, std::pair<double, double>{ 0.05, 0.05 
            } ,ui::RelativeLayout{ layout, 0.0, 0.2, 1.0, 0.6 }} {

        auto focus_helper = ui::FocusHelper{ 1 };

        m_main_grid.add<ui::Label>(
                service_provider, "Select MultiPlayer Mode", service_provider->font_manager().get(FontId::Default),
                Color::white(), std::pair<double, double>{ 0.3, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );


        constexpr auto button_size = utils::get_orientation() == utils::Orientation::Landscape
                                             ? std::pair<double, double>{ 0.15, 0.85 }
                                             : std::pair<double, double>{ 0.5, 0.85 };
        constexpr auto button_alignment =
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center };
        constexpr auto button_margins = utils::get_orientation() == utils::Orientation::Landscape
                                                ? std::pair<double, double>{ 0.1, 0.1 }
                                                : std::pair<double, double>{ 0.2, 0.2 };

        const auto local_button_id = m_main_grid.add<ui::TextButton>(
                service_provider, "Local", service_provider->font_manager().get(FontId::Default), Color::white(),
                focus_helper.focus_id(),
                [this](const ui::TextButton&) -> bool {
                    m_next_command = Command::LocalMultiPlayer;
                    return false;
                },
                button_size, button_alignment, button_margins
        );
        m_main_grid.get<ui::TextButton>(local_button_id)->disable();

        m_main_grid.add<ui::TextButton>(
                service_provider, "Online", service_provider->font_manager().get(FontId::Default), Color::white(),
                focus_helper.focus_id(),
                [this](const ui::TextButton&) -> bool {
                    m_next_command = Command::OnlineMultiPlayer;
                    return false;
                },
                button_size, button_alignment, button_margins
        );

        const auto ai_button_id = m_main_grid.add<ui::TextButton>(
                service_provider, "vs AI", service_provider->font_manager().get(FontId::Default), Color::white(),
                focus_helper.focus_id(),
                [this](const ui::TextButton&) -> bool {
                    m_next_command = Command::AIMultiPlayer;
                    return false;
                },
                button_size, button_alignment, button_margins
        );
        m_main_grid.get<ui::TextButton>(ai_button_id)->disable();

        m_main_grid.add<ui::TextButton>(
                service_provider, "Return", service_provider->font_manager().get(FontId::Default), Color::white(),
                focus_helper.focus_id(),
                [this](const ui::TextButton&) -> bool {
                    m_next_command = Command::Return;
                    return false;
                },
                button_size, button_alignment, button_margins
        );
    }

    [[nodiscard]] Scene::UpdateResult MultiPlayerMenu::update() {
        m_main_grid.update();

        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::LocalMultiPlayer:
                    return UpdateResult{
                        SceneUpdate::StopUpdating, Scene::Switch{ SceneId::LocalMultiPlayerGame,
                                                                 ui::FullScreenLayout{ m_service_provider->window() } }
                    };
                case Command::OnlineMultiPlayer:
                    // perform a push and reset the command, so that the music keeps playing the entire time
                    m_next_command = std::nullopt;
                    return UpdateResult{
                        SceneUpdate::StopUpdating,
                        Scene::Push{ SceneId::OnlineLobby, ui::FullScreenLayout{ m_service_provider->window() } }
                    };
                case Command::AIMultiPlayer:
                    return UpdateResult{
                        SceneUpdate::StopUpdating, Scene::Switch{ SceneId::AIMultiPlayerGame,
                                                                 ui::FullScreenLayout{ m_service_provider->window() } }
                    };
                case Command::Return:
                    return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
                default:
                    UNREACHABLE();
            }
        }
        return UpdateResult{ SceneUpdate::StopUpdating, std::nullopt };
    }

    void MultiPlayerMenu::render(const ServiceProvider& service_provider) {

        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black());

        m_main_grid.render(service_provider);
    }

    bool
    MultiPlayerMenu::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {
        if (m_main_grid.handle_event(input_manager, event)) {
            return true;
        }

        const auto navigation_event = input_manager->get_navigation_event(event);

        if (navigation_event == input::NavigationEvent::BACK) {
            m_next_command = Command::Return;
            return true;
        }
        return false;
    }

} // namespace scenes
