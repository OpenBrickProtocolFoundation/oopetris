#pragma once

#include "application.hpp"
#include "game_manager.hpp"
#include "settings.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <fstream>
#include <spdlog/spdlog.h>

struct TetrisApplication : public Application {
private:
    static constexpr auto settings_filename = "settings.json";

    std::vector<std::unique_ptr<GameManager>> m_game_managers;
    std::vector<std::unique_ptr<Input>> m_inputs;
    Settings m_settings;
    std::unique_ptr<RecordingWriter> m_recording_writer;

public:
    static constexpr int width = 800;
    static constexpr int height = 600;

    explicit TetrisApplication(CommandLineArguments command_line_arguments)
        : Application{ "TetrisApplication", WindowPosition::Centered, width, height,
                       std::move(command_line_arguments) } {
        const auto settings = load_settings();
        if (settings.has_value()) {
            m_settings = *settings;
            spdlog::info("settings loaded");
        } else {
            spdlog::error("unable to load settings from \"{}\"", settings_filename);
            spdlog::warn("applying default settings");
        }

        static constexpr auto num_players = 1;

        const auto is_recording = this->command_line_arguments().recording_path.has_value();
        auto recording = [&]() -> tl::optional<RecordingReader> {
            if (is_recording) {
                return RecordingReader{ *(this->command_line_arguments().recording_path) };
            } else {
                return tl::nullopt;
            }
        }();

        const auto random_seed = Random::generate_seed();
        for (int i = 0; i < num_players; ++i) {
            const auto this_players_seed = [&]() {
                if (is_recording) {
                    return recording->tetrion_headers().at(i).seed;
                } else {
                    return random_seed;
                }
            }();

            spdlog::info("seed for player {}: {}", i + 1, this_players_seed);


            if (not is_recording) {
                static constexpr auto recordings_directory = "recordings";
                const auto recording_directory_path = std::filesystem::path{ recordings_directory };
                if (not std::filesystem::exists(recording_directory_path)) {
                    std::filesystem::create_directory(recording_directory_path);
                }
                const auto filename = fmt::format("{}.rec", utils::current_date_time_iso8601());
                const auto file_path = recording_directory_path / filename;

                // todo: add more headers when there are more players
                auto tetrion_headers = std::vector<Recording::TetrionHeader>{
                    Recording::TetrionHeader{
                                             .seed{ this_players_seed },
                                             .starting_level{ this->command_line_arguments().starting_level },
                                             }
                };
                m_recording_writer = std::make_unique<RecordingWriter>(file_path, std::move(tetrion_headers));
            };

            const auto recording_writer_optional = [&]() -> tl::optional<RecordingWriter*> {
                if (m_recording_writer) {
                    return m_recording_writer.get();
                }
                return tl::nullopt;
            }();

            const auto starting_level =
                    (is_recording ? recording->tetrion_headers().at(i).starting_level
                                  : this->command_line_arguments().starting_level);

            spdlog::info("starting level for player {}: {}", i + 1, starting_level);

            m_game_managers.push_back(
                    std::make_unique<GameManager>(this_players_seed, starting_level, recording_writer_optional)
            );

            auto on_event_callback = [&]() -> Input::OnEventCallback {
                if (m_recording_writer) {
                    return [i, this](InputEvent event) {
                        spdlog::info(
                                "recording event {} at step {}", magic_enum::enum_name(event),
                                Application::simulation_step_index()
                        );
                        m_recording_writer->add_event(static_cast<u8>(i), Application::simulation_step_index(), event);
                    };
                } else {
                    return [](InputEvent) {
                        // we don't want to record events => do nothing
                    };
                }
            }();

            if (is_recording) {
                m_inputs.push_back(create_input(
                        ReplayControls{ *recording }, m_game_managers.back().get(), std::move(on_event_callback)
                ));
            } else {
                m_inputs.push_back(create_input(
                        std::move(m_settings.controls.at(i)), m_game_managers.back().get(), std::move(on_event_callback)
                ));
            }
        }
        for (const auto& game_manager : m_game_managers) {
            game_manager->spawn_next_tetromino();
        }
    }

protected:
    void update() override {
        for (const auto& input : m_inputs) {
            input->update();
        }

        for (const auto& game_manager : m_game_managers) {
            game_manager->update();
        }
    }

    void render() const override {
        Application::render(); // call parent function to clear the screen
        for (const auto& game_manager : m_game_managers) {
            game_manager->render(*this);
        }
    }

private:
    std::unique_ptr<Input>
    create_input(Controls controls, GameManager* associated_game_manager, Input::OnEventCallback on_event_callback) {
        return std::visit(
                overloaded{ [&](KeyboardControls& keyboard_controls) -> std::unique_ptr<Input> {
                               auto keyboard_input = std::make_unique<KeyboardInput>(
                                       associated_game_manager, std::move(on_event_callback), keyboard_controls
                               );
                               m_event_dispatcher.register_listener(keyboard_input.get());
                               return keyboard_input;
                           },
                            [&](ReplayControls& replay_controls) -> std::unique_ptr<Input> {
                                return std::make_unique<ReplayInput>(
                                        associated_game_manager, std::move(on_event_callback),
                                        std::move(replay_controls.recording)

                                );
                            } },
                controls
        );
    }

    static tl::optional<Settings> load_settings() try {
        std::ifstream settings_file{ settings_filename };
        Settings settings = nlohmann::json::parse(settings_file);
        return settings;
    } catch (...) {
        return {};
    }
};
