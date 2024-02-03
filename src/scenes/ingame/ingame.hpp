#pragma once

#include "../../command_line_arguments.hpp"
#include "../../recording.hpp"
#include "../scene.hpp"

namespace scenes {

    struct Ingame : public Scene {
    private:
        using TetrionHeaders = std::vector<Recording::TetrionHeader>;

        enum class NextScene { Pause, Settings };

        std::vector<std::unique_ptr<ClockSource>> m_clock_sources;
        std::vector<SimulationStep> m_simulation_step_indices;
        std::vector<std::unique_ptr<Tetrion>> m_tetrions;
        std::vector<std::unique_ptr<Input>> m_inputs;
        std::unique_ptr<RecordingWriter> m_recording_writer;
        std::unique_ptr<RecordingReader> m_recording_reader;
        tl::optional<NextScene> m_next_scene{};
        bool m_is_paused{ false };

    public:
        explicit Ingame(ServiceProvider* service_provider);

        [[nodiscard]] UpdateResult update() override;
        void render(const ServiceProvider& service_provider) override;
        [[nodiscard]] bool handle_event(const SDL_Event& event, const Window* window) override;

    private:
        [[nodiscard]] std::unique_ptr<Input>
        create_input(Controls controls, Tetrion* associated_tetrion, Input::OnEventCallback on_event_callback);

        [[nodiscard]] static std::unique_ptr<Input> create_replay_input(
                RecordingReader* recording_reader,
                Tetrion* associated_tetrion,
                Input::OnEventCallback on_event_callback
        );

        [[nodiscard]] Input::OnEventCallback create_on_event_callback(int tetrion_index);

        [[nodiscard]] bool is_replay_mode() const;

        [[nodiscard]] bool game_is_recorded() const;

        [[nodiscard]] Random::Seed seed_for_tetrion(u8 tetrion_index, Random::Seed common_seed) const;

        [[nodiscard]] auto starting_level_for_tetrion(u8 tetrion_index) const
                -> decltype(CommandLineArguments::starting_level);

        [[nodiscard]] TetrionHeaders create_tetrion_headers(std::span<const Random::Seed> seeds) const;

        [[nodiscard]] static std::unique_ptr<RecordingWriter> create_recording_writer(TetrionHeaders tetrion_headers);

        [[nodiscard]] std::vector<Random::Seed> create_seeds(u8 num_tetrions) const;

        [[nodiscard]] tl::optional<RecordingWriter*> recording_writer_optional();

        [[nodiscard]] bool is_game_over() const;
    };

} // namespace scenes
