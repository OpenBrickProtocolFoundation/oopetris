#pragma once

#include <recordings/utility/recording.hpp>

#include "core/helper/expected.hpp"
#include "helper/windows.hpp"
#include "input/input_creator.hpp"
#include "input/replay_input.hpp"
#include "simulated_tetrion.hpp"

struct Simulation {
private:
    using TetrionHeaders = std::vector<recorder::TetrionHeader>;

    SimulationStep m_simulation_step_index{ 0 };
    std::unique_ptr<SimulatedTetrion> m_tetrion;
    std::shared_ptr<input::ReplayGameInput> m_input;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit Simulation(
            const std::shared_ptr<input::ReplayGameInput>& input,
            const tetrion::StartingParameters& starting_parameters
    );

    OOPETRIS_GRAPHICS_EXPORTED static helper::expected<Simulation, std::string> get_replay_simulation(
            std::filesystem::path& recording_path
    );

    OOPETRIS_GRAPHICS_EXPORTED void update();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool is_game_finished() const;
};
