#include "online_lobby.hpp"

#include <core/helper/errors.hpp>
#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>

#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "helper/platform.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"

#include "ui/components/textinput.hpp"
#include "ui/layout.hpp"
#include "ui/layouts/scroll_layout.hpp"

namespace scenes {

    OnlineLobby::OnlineLobby(ServiceProvider* service_provider, const ui::Layout& layout)
        : Scene{ service_provider, layout },
          m_main_layout{ 
                utils::SizeIdentity<3>(),
                0,
                ui::Direction::Vertical,
                { 0.1, 0.9 },
                ui::AbsolutMargin{ 10 },
                std::pair<double, double>{ 0.05, 0.03 },
                layout
          } {
        auto focus_helper = ui::FocusHelper{ 1 };

        m_main_layout.add<ui::Label>(
                service_provider, "Select Lobby to play in", service_provider->font_manager().get(FontId::Default),
                Color::white(), std::pair<double, double>{ 0.5, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        const auto scroll_layout_index = m_main_layout.add<ui::ScrollLayout>(
                service_provider, focus_helper.focus_id(), ui::AbsolutMargin{ 10 },
                std::pair<double, double>{ 0.05, 0.03 }
        );

        auto* scroll_layout = m_main_layout.get<ui::ScrollLayout>(scroll_layout_index);

        for (auto i = 0; i < 7; ++i) {
            if (i == 2) {
                scroll_layout->add<ui::TextInput>(
                        ui::RelativeItemSize{ scroll_layout->layout(), 0.2 }, service_provider,
                        service_provider->font_manager().get(FontId::Symbola), Color::white(), focus_helper.focus_id(),
                        std::pair<double, double>{ 0.9, 0.9 },
                        ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                        ui::TextInputMode::Scroll
                );
            } else {
                scroll_layout->add<ui::TextButton>(
                        ui::RelativeItemSize{ scroll_layout->layout(), 0.2 }, service_provider,
                        fmt::format("Button Nr.: {}", i), service_provider->font_manager().get(FontId::Default),
                        Color::white(), focus_helper.focus_id(),
                        [i](const ui::TextButton&) -> bool {
                            spdlog::info("Pressed button: {}", i);
                            return false;
                        },
                        std::pair<double, double>{ 0.8, 1.0 },
                        ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                        std::pair<double, double>{ 0.1, 0.2 }
                );
            }
        }

        constexpr auto button_size = utils::get_orientation() == utils::Orientation::Landscape
                                             ? std::pair<double, double>{ 0.15, 0.85 }
                                             : std::pair<double, double>{ 0.5, 0.85 };
        constexpr auto button_alignment =
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center };
        constexpr auto button_margins = utils::get_orientation() == utils::Orientation::Landscape
                                                ? std::pair<double, double>{ 0.1, 0.1 }
                                                : std::pair<double, double>{ 0.2, 0.2 };

        m_main_layout.add<ui::TextButton>(
                service_provider, "Return", service_provider->font_manager().get(FontId::Default), Color::white(),
                focus_helper.focus_id(),
                [this](const ui::TextButton&) -> bool {
                    m_next_command = Command::Return;
                    return false;
                },
                button_size, button_alignment, button_margins
        );
    }

    [[nodiscard]] Scene::UpdateResult OnlineLobby::update() {
        m_main_layout.update();

        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::Play:
                    return UpdateResult{
                        SceneUpdate::StopUpdating, Scene::Switch{ SceneId::OnlineMultiplayerGame,
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

    void OnlineLobby::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black());

        m_main_layout.render(service_provider);
    }

    bool OnlineLobby::handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) {
        // description of intentional behaviour of this scene, even if it seems off:
        // the return button or the scroll layout can have the focus, if the scroll_layout has the focus, it can be scrolled by the scroll wheel and you can move around the focused item of the scroll_layout with up and down, but not with TAB, with tab you can change the focus to the return button, where you can't use the scroll wheel or up / down to change the scroll items, but you still can use click events, they are not affected by focus

        if (const auto event_result = m_main_layout.handle_event(input_manager, event)) {

            if (const auto additional = event_result.get_additional(); additional.has_value()) {
                const auto value = additional.value();

                if (value.handle_type == ui::EventHandleType::RequestAction) {

                    if (auto text_input = utils::is_child_class<ui::TextInput>(value.widget); text_input.has_value()) {
                        spdlog::info("Pressed Enter on TextInput  {}", text_input.value()->get_text());

                        if (text_input.value()->has_focus()) {
                            text_input.value()->unfocus();
                        }
                        return true;
                    }


                    throw std::runtime_error("Requested action on unknown widget, this is a fatal error");
                }

                throw helper::FatalError(
                        fmt::format("Unsupported Handle Type: {}", magic_enum::enum_name(value.handle_type))
                );
            }

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
