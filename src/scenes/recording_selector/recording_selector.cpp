#include "recording_selector.hpp"
#include "graphics/window.hpp"
#include "helper/constants.hpp"
#include "manager/music_manager.hpp"
#include "manager/resource_manager.hpp"
#include "ui/components/textinput.hpp"
#include "ui/layout.hpp"
#include "ui/layouts/scroll_layout.hpp"

namespace scenes {

    RecordingSelector::RecordingSelector(ServiceProvider* service_provider, const ui::Layout& layout)
        : Scene{ service_provider, layout },
          m_main_layout{ 
                utils::size_t_identity<3>(),
                0,
                ui::Direction::Vertical,
                { 0.1, 0.9 },
                ui::AbsolutMargin{ 10 },
                std::pair<double, double>{ 0.05, 0.03 },
                layout
          } {

        auto focus_helper = ui::FocusHelper{ 1 };

        m_main_layout.add<ui::Label>(
                service_provider, "Select Recording to replay", service_provider->fonts().get(FontId::Default),
                Color::white(), std::pair<double, double>{ 0.5, 1.0 },
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
        );

        const auto scroll_layout_index = m_main_layout.add<ui::ScrollLayout>(
                service_provider, focus_helper.focus_id(), ui::AbsolutMargin{ 10 },
                std::pair<double, double>{ 0.05, 0.03 }
        );

        auto* scroll_layout = m_main_layout.get<ui::ScrollLayout>(scroll_layout_index);

        for (auto i = 0; i < 3; ++i) {
            if (i == 2) {
                scroll_layout->add<ui::TextInput>(
                        ui::RelativeItemSize{ scroll_layout->layout(), 0.2 }, service_provider,
                        service_provider->fonts().get(FontId::Symbola), Color::white(), focus_helper.focus_id(),
                        std::pair<double, double>{ 0.9, 0.9 },
                        ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
                );
            } else {
                scroll_layout->add<ui::Button>(
                        ui::RelativeItemSize{ scroll_layout->layout(), 0.2 }, service_provider,
                        fmt::format("Button Nr.: {}", i), service_provider->fonts().get(FontId::Default),
                        Color::white(), focus_helper.focus_id(),
                        [i](const ui::Button&) { std::cout << "Pressed button: " << i << "\n"; },
                        std::pair<double, double>{ 0.8, 1.0 },
                        ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center },
                        std::pair<double, double>{ 0.1, 0.2 }
                );
            }
        }

        constexpr auto button_size = utils::device_orientation() == utils::Orientation::Landscape
                                             ? std::pair<double, double>{ 0.15, 0.85 }
                                             : std::pair<double, double>{ 0.5, 0.85 };
        constexpr auto button_alignment =
                ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center };
        constexpr auto button_margins = utils::device_orientation() == utils::Orientation::Landscape
                                                ? std::pair<double, double>{ 0.1, 0.1 }
                                                : std::pair<double, double>{ 0.2, 0.2 };

        m_main_layout.add<ui::Button>(
                service_provider, "Return", service_provider->fonts().get(FontId::Default), Color::white(),
                focus_helper.focus_id(), [this](const ui::Button&) { m_next_command = Command::Return; }, button_size,
                button_alignment, button_margins
        );
    }

    [[nodiscard]] Scene::UpdateResult RecordingSelector::update() {
        m_main_layout.update();

        if (m_next_command.has_value()) {
            switch (m_next_command.value()) {
                case Command::Play:
                    return UpdateResult{
                        SceneUpdate::StopUpdating, Scene::Switch{SceneId::OnlineMultiplayerGame,
                                                                 ui::FullScreenLayout{ m_service_provider->window() }}
                    };
                case Command::Return:
                    return UpdateResult{ SceneUpdate::StopUpdating, Scene::Pop{} };
                default:
                    utils::unreachable();
            }
        }
        return UpdateResult{ SceneUpdate::StopUpdating, helper::nullopt };
    }

    void RecordingSelector::render(const ServiceProvider& service_provider) {
        service_provider.renderer().draw_rect_filled(get_layout().get_rect(), Color::black());

        m_main_layout.render(service_provider);
    }

    bool RecordingSelector::handle_event(const SDL_Event& event, const Window* window) {
        // description of intentional behaviour of this scene, even if it seems off:
        // the return button or the scroll layout can have the focus, if the scroll_layout has the focus, it can be scrolled by the scroll wheel and you can move around the focused item of the scroll_layout with up and down, but not with TAB, with tab you can change the focus to the return button, where you can't use the scroll wheel or up / down to change the scroll items, but you still can use click events, they are not affected by focus

        //TODO: make this simpler and refactor scroll_layout, since it's not a full layout but a hybrid!
        auto* const scroll_layout = m_main_layout.get<ui::ScrollLayout>(1);
        if (scroll_layout->has_focus()) {
            if (const auto event_result = scroll_layout->handle_event(event, window); event_result) {
                if (event_result.has_additional()) {
                    //TODO: urgent
                    // std::ignore = m_main_layout.handle_event_result(event_result.get_additional(), scroll_layout);
                }

                return true;
            }
        }

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
