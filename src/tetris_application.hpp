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
    std::unique_ptr<RecordingReader> m_recording_reader;

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

        static constexpr auto num_players = u8{ 1 };

        const auto is_recording = this->command_line_arguments().recording_path.has_value();
        if (is_recording) {
            m_recording_reader = std::make_unique<RecordingReader>(*(this->command_line_arguments().recording_path));
        }

        const auto random_seed = Random::generate_seed();
        for (u8 tetrion_index = 0; tetrion_index < num_players; ++tetrion_index) {
            const auto this_players_seed = [&]() {
                if (is_recording) {
                    return m_recording_reader->tetrion_headers().at(tetrion_index).seed;
                } else {
                    return random_seed;
                }
            }();

            spdlog::info("seed for player {}: {}", tetrion_index + 1, this_players_seed);


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
                    (is_recording ? m_recording_reader->tetrion_headers().at(tetrion_index).starting_level
                                  : this->command_line_arguments().starting_level);

            spdlog::info("starting level for player {}: {}", tetrion_index + 1, starting_level);

            m_game_managers.push_back(
                    std::make_unique<GameManager>(this_players_seed, starting_level, recording_writer_optional)
            );

            auto on_event_callback = create_on_event_callback(tetrion_index);

            const auto game_manager = m_game_managers.back().get();
            if (is_recording) {
                m_inputs.push_back(
                        create_recording_input(tetrion_index, m_recording_reader.get(), game_manager, [](InputEvent) {})
                );
            } else {
                m_inputs.push_back(create_input(
                        std::move(m_settings.controls.at(tetrion_index)), game_manager, std::move(on_event_callback)
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
    [[nodiscard]] std::unique_ptr<Input>
    create_input(Controls controls, GameManager* associated_game_manager, Input::OnEventCallback on_event_callback) {
        return std::visit(
                overloaded{
                        [&](KeyboardControls& keyboard_controls) -> std::unique_ptr<Input> {
                            auto keyboard_input = std::make_unique<KeyboardInput>(
                                    associated_game_manager, std::move(on_event_callback), keyboard_controls
                            );
                            m_event_dispatcher.register_listener(keyboard_input.get());
                            return keyboard_input;
                        },
                },
                controls
        );
    }

    [[nodiscard]] static std::unique_ptr<Input> create_recording_input(
            const u8 tetrion_index,
            RecordingReader* const recording_reader,
            GameManager* const associated_game_manager,
            Input::OnEventCallback on_event_callback
    ) {
        return std::make_unique<ReplayInput>(
                associated_game_manager, tetrion_index, std::move(on_event_callback), recording_reader
        );
    }

    [[nodiscard]] Input::OnEventCallback create_on_event_callback(const int tetrion_index) {
        if (m_recording_writer) {
            return [tetrion_index, this](InputEvent event) {
                m_recording_writer->add_event(
                        static_cast<u8>(tetrion_index), Application::simulation_step_index(), event
                );
            };
        } else {
            return Input::OnEventCallback{}; // empty std::function object
        }
    }

    static tl::optional<Settings> load_settings() try {
        std::ifstream settings_file{ settings_filename };
        Settings settings = nlohmann::json::parse(settings_file);
        return settings;
    } catch (...) {
        return {};
    }
};
