
#include <core/helper/expected.hpp>
#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>

#include "input/replay_input.hpp"
#include "simulation.hpp"

#include <spdlog/spdlog.h>


Simulation::Simulation(
        const std::shared_ptr<input::ReplayGameInput>& input,
        const tetrion::StartingParameters& starting_parameters
)
    : m_input{ input } {


    spdlog::info("[simulation] starting level for tetrion {}", starting_parameters.starting_level);

    m_tetrion = std::make_unique<SimulatedTetrion>(
            starting_parameters.tetrion_index, starting_parameters.seed, starting_parameters.starting_level, nullptr,
            starting_parameters.recording_writer
    );

    m_tetrion->spawn_next_tetromino(0);

    m_input->set_target_tetrion(m_tetrion.get());
    if (starting_parameters.recording_writer.has_value()) {
        const auto recording_writer = starting_parameters.recording_writer.value();
        const auto tetrion_index = starting_parameters.tetrion_index;
        m_input->set_event_callback([recording_writer,
                                     tetrion_index](InputEvent event, SimulationStep simulation_step_index) {
            spdlog::debug("event: {} (step {})", magic_enum::enum_name(event), simulation_step_index);

            //TODO(Totto): Remove all occurrences of std::ignore, where we shouldn't ignore this return value
            std::ignore = recording_writer->add_record(tetrion_index, simulation_step_index, event);
        });
    }
}

helper::expected<Simulation, std::string> Simulation::get_replay_simulation(std::filesystem::path& recording_path) {

    //TODO(Totto): Support multiple tetrions to be in the recorded file and simulated

    auto maybe_recording_reader = recorder::RecordingReader::from_path(recording_path);

    if (not maybe_recording_reader.has_value()) {
        return helper::unexpected<std::string>{
            fmt::format("an error occurred while reading recording: {}", maybe_recording_reader.error())
        };
    }

    const auto recording_reader =
            std::make_shared<recorder::RecordingReader>(std::move(maybe_recording_reader.value()));


    const auto tetrion_headers = recording_reader->tetrion_headers();

    if (tetrion_headers.size() != 1) {
        return helper::unexpected<std::string>{
            fmt::format("Expected 1 recording in the recording file, but got : {}", tetrion_headers.size())
        };
    }

    const auto tetrion_index = 0;

    auto input = std::make_shared<input::ReplayGameInput>(recording_reader, nullptr);

    const auto& header = tetrion_headers.at(tetrion_index);

    const auto seed = header.seed;
    const auto starting_level = header.starting_level;

    const tetrion::StartingParameters starting_parameters = { 0, seed, starting_level, tetrion_index, std::nullopt };

    return Simulation{ input, starting_parameters };
}


void Simulation::update() {
    if (is_game_finished()) {
        return;
    }

    ++m_simulation_step_index;
    m_input->update(m_simulation_step_index);
    m_tetrion->update_step(m_simulation_step_index);
    m_input->late_update(m_simulation_step_index);
}

[[nodiscard]] bool Simulation::is_game_finished() const {
    if (m_tetrion->is_game_over()) {
        return true;
    };

    const auto input_as_replay = utils::is_child_class<input::ReplayGameInput>(m_input);
    if (input_as_replay.has_value()) {
        return input_as_replay.value()->is_end_of_recording();
    }

    return false;
}
