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
        auto recording = [&]() -> tl::optional<Recording> {
            if (is_recording) {
                return Recording{ *(this->command_line_arguments().recording_path) };
            } else {
                return tl::nullopt;
            }
        }();

        const auto random_seed = (is_recording ? recording->seed() : Random::generate_seed());
        for (int i = 0; i < num_players; ++i) {
            spdlog::info("seed for player {}: {}", i + 1, random_seed);
            const auto record_game = not is_recording;
            m_game_managers.push_back(std::make_unique<GameManager>(
                    random_seed, this->command_line_arguments().starting_level, record_game
            ));

            if (is_recording) {
                m_inputs.push_back(create_input(ReplayControls{ std::move(*recording) }, m_game_managers.back().get()));
            } else {
                m_inputs.push_back(create_input(m_settings.controls.at(i), m_game_managers.back().get()));
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
    std::unique_ptr<Input> create_input(Controls controls, GameManager* associated_game_manager) {
        return std::visit(
                overloaded{ [&](KeyboardControls& keyboard_controls) -> std::unique_ptr<Input> {
                               auto keyboard_input =
                                       std::make_unique<KeyboardInput>(associated_game_manager, keyboard_controls);
                               m_event_dispatcher.register_listener(keyboard_input.get());
                               return keyboard_input;
                           },
                            [&](ReplayControls& replay_controls) -> std::unique_ptr<Input> {
                                return std::make_unique<ReplayInput>(
                                        associated_game_manager, std::move(replay_controls.recording)
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
