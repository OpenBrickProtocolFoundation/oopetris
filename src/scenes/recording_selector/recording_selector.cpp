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

        add_all_recordings();

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


        if (m_main_layout.handle_event(event, window)) {
            return true;
        }

        if (utils::event_is_action(event, utils::CrossPlatformAction::CLOSE)) {
            m_next_command = Command::Return;
            return true;
        }
        return false;
    }

    void RecordingSelector::add_all_recordings() {

        std::vector<data::RecordingMetadata> metadata_vector{};

        const auto recording_directory_path = utils::get_root_folder() / constants::recordings_directory;

        for (const auto& file : std::filesystem::recursive_directory_iterator(recording_directory_path)) {
            if (recorder::RecordingReader::is_header_valid(file.path())) {
                metadata_vector.emplace_back(file.path(), data::RecordingSource::Folder);
            } else {
                spdlog::info("While scanning recordings folder: file {} is not a recording", file.path().string());
            }
        }

        if (const auto recording_path = m_service_provider->command_line_arguments().recording_path;
            recording_path.has_value()) {

            const auto recording_path_cl = std::filesystem::path(recording_path.value());

            if (recorder::RecordingReader::is_header_valid(recording_path_cl)) {
                metadata_vector.emplace_back(recording_path_cl, data::RecordingSource::Commandline);
            } else {
                spdlog::error(
                        "Recording file specified by the commandline is not a recording", recording_path_cl.string()
                );
            }
        }

        auto* scroll_layout = m_main_layout.get<ui::ScrollLayout>(1);

        if (scroll_layout->widget_count() != 0) {
            scroll_layout->clear();
        }

        auto focus_helper = ui::FocusHelper{ 3 };

        for (const auto& metadata : metadata_vector) {
            //TODO: add custom widget that renders those metadatas
        }
    }

} // namespace scenes
