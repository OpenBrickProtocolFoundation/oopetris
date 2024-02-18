#include "online_lobby.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "ui/layout.hpp"

namespace scenes {

    OnlineLobby::OnlineLobby(ServiceProvider* service_provider, const ui::Layout& layout)
        : Scene{ service_provider, layout },
          m_main_layout {
        ui::Direction::Vertical, std::array<double, 2>{ 0.1, 0.9 }, ui::AbsolutMargin{ 10 }, std::pair<double, double>{ 0.05, 0.03 }, layout
    }{

        auto id_helper = ui::IDHelper{};

        m_main_layout.add<ui::Label>(
                id_helper.index(), service_provider, "Select Lobby to play in",
                service_provider->fonts().get(FontId::Default), Color::white(), std::pair<double, double>{ 0.5, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        //TODO:
        m_main_layout.add<ui::Label>(
                id_helper.index(), service_provider, "TODO", service_provider->fonts().get(FontId::Default),
                Color::white(), std::pair<double, double>{ 0.5, 1.0 },
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

        m_main_layout.add<ui::Button>(
                id_helper.index(), service_provider, "Return", service_provider->fonts().get(FontId::Default),
                Color::white(), id_helper.focus_id(), [this](const ui::Button&) { m_next_command = Command::Return; },
                button_size, button_alignment, button_margins
        );
    }

    [[nodiscard]] Scene::UpdateResult OnlineLobby::update() {
        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::Play:
                    return UpdateResult{
                        SceneUpdate::ContinueUpdating,
                        Scene::Switch{SceneId::OnlineMultiplayerGame,
                                      ui::FullScreenLayout{ m_service_provider->window() }}
                    };
                case Command::Return:
                    return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::ContinueUpdating, helpers::nullopt };
    }

    void OnlineLobby::render(const ServiceProvider& service_provider) {

        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black());

        m_main_layout.render(service_provider);
    }

    bool OnlineLobby::handle_event(const SDL_Event& event, const Window* window) {
        if (m_main_layout.handle_event(event, window)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_next_command = Command::Return;
            return true;
        }
        return false;
    }

} // namespace scenes
