#pragma once

#include "application.hpp"
#include "settings.hpp"
#include "tetrion.hpp"
#include "tetromino_type.hpp"
#include <cassert>
#include <fstream>
#include <span>
#include <spdlog/spdlog.h>

struct TetrisApplication : public Application {
private:
    using TetrionHeaders = std::vector<Recording::TetrionHeader>;

    static constexpr auto settings_filename = "settings.json";

    std::vector<std::unique_ptr<Tetrion>> m_tetrions;
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
        try_load_settings();
        static constexpr auto num_tetrions = u8{ 1 };

        if (is_replay_mode()) {
            m_recording_reader = std::make_unique<RecordingReader>(*(this->command_line_arguments().recording_path));
        }

        const auto seeds = create_seeds(num_tetrions);

        if (game_is_recorded()) {
            const auto seeds_span = std::span{ seeds.data(), seeds.size() };
            m_recording_writer = create_recording_writer(create_tetrion_headers(seeds_span));
        }

        for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
            const auto recording_writer_optional = [&]() -> tl::optional<RecordingWriter*> {
                if (m_recording_writer) {
                    return m_recording_writer.get();
                }
                return tl::nullopt;
            }();

            const auto starting_level = starting_level_for_tetrion(tetrion_index);
            spdlog::info("starting level for tetrion {}: {}", tetrion_index, starting_level);

            m_tetrions.push_back(
                    std::make_unique<Tetrion>(seeds.at(tetrion_index), starting_level, recording_writer_optional)
            );

            auto on_event_callback = create_on_event_callback(tetrion_index);

            const auto tetrion_pointer = m_tetrions.back().get();
            if (is_replay_mode()) {
                m_inputs.push_back(create_recording_input(
                        tetrion_index, m_recording_reader.get(), tetrion_pointer, [](InputEvent) {}
                ));
            } else {
                m_inputs.push_back(create_input(
                        m_settings.controls.at(tetrion_index), tetrion_pointer, std::move(on_event_callback)
                ));
            }
        }
        for (const auto& tetrion : m_tetrions) {
            tetrion->spawn_next_tetromino();
        }
    }

protected:
    void update() override {
        for (const auto& input : m_inputs) {
            input->update();
        }

        for (const auto& tetrion : m_tetrions) {
            tetrion->update();
        }
    }

    void render() const override {
        Application::render(); // call parent function to clear the screen
        for (const auto& tetrion : m_tetrions) {
            tetrion->render(*this);
        }
    }

private:
    [[nodiscard]] std::unique_ptr<Input>
    create_input(Controls controls, Tetrion* associated_tetrion, Input::OnEventCallback on_event_callback) {
        return std::visit(
                overloaded{
                        [&](KeyboardControls& keyboard_controls) -> std::unique_ptr<Input> {
                            auto keyboard_input = std::make_unique<KeyboardInput>(
                                    associated_tetrion, std::move(on_event_callback), keyboard_controls
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
            Tetrion* const associated_tetrion,
            Input::OnEventCallback on_event_callback
    ) {
        return std::make_unique<ReplayInput>(
                associated_tetrion, tetrion_index, std::move(on_event_callback), recording_reader
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

    void try_load_settings() try {
        std::ifstream settings_file{ settings_filename };
        m_settings = nlohmann::json::parse(settings_file);
        spdlog::info("settings loaded");
    } catch (...) {
        spdlog::error("unable to load settings from \"{}\"", settings_filename);
        spdlog::warn("applying default settings");
    }

    [[nodiscard]] bool is_replay_mode() const {
        return this->command_line_arguments().recording_path.has_value();
    }

    [[nodiscard]] bool game_is_recorded() const {
        return not is_replay_mode();
    }

    [[nodiscard]] Random::Seed seed_for_tetrion(const u8 tetrion_index, const Random::Seed common_seed) const {
        return (is_replay_mode() ? m_recording_reader->tetrion_headers().at(tetrion_index).seed : common_seed);
    }

    [[nodiscard]] auto starting_level_for_tetrion(const u8 tetrion_index) const
            -> decltype(CommandLineArguments::starting_level) {
        return is_replay_mode() ? m_recording_reader->tetrion_headers().at(tetrion_index).starting_level
                                : this->command_line_arguments().starting_level;
    }

    [[nodiscard]] TetrionHeaders create_tetrion_headers(const std::span<const Random::Seed> seeds) const {
        const auto num_tetrions = seeds.size();
        auto headers = TetrionHeaders{};
        headers.reserve(num_tetrions);
        const auto common_seed = Random::generate_seed();
        for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
            const auto tetrion_seed = seed_for_tetrion(tetrion_index, common_seed);
            const auto starting_level = starting_level_for_tetrion(tetrion_index);
            headers.push_back(Recording::TetrionHeader{ .seed{ tetrion_seed }, .starting_level{ starting_level } });
            spdlog::info("seed for tetrion {}: {}", tetrion_index, tetrion_seed);
            spdlog::info("starting level for tetrion {}: {}", tetrion_index, starting_level);
        }
        return headers;
    }

    [[nodiscard]] static std::unique_ptr<RecordingWriter> create_recording_writer(TetrionHeaders tetrion_headers) {
        static constexpr auto recordings_directory = "recordings";
        const auto recording_directory_path = std::filesystem::path{ recordings_directory };
        if (not std::filesystem::exists(recording_directory_path)) {
            std::filesystem::create_directory(recording_directory_path);
        }
        const auto filename = fmt::format("{}.rec", utils::current_date_time_iso8601());
        const auto file_path = recording_directory_path / filename;

        return std::make_unique<RecordingWriter>(file_path, std::move(tetrion_headers));
    }

    [[nodiscard]] std::vector<Random::Seed> create_seeds(const u8 num_tetrions) const {
        auto seeds = std::vector<Random::Seed>{};
        seeds.reserve(num_tetrions);
        const auto common_seed = Random::generate_seed();
        for (u8 tetrion_index = 0; tetrion_index < num_tetrions; ++tetrion_index) {
            seeds.push_back(seed_for_tetrion(tetrion_index, common_seed));
        }
        return seeds;
    }
};
