#include "replay_input.hpp"
#include "game/tetrion.hpp"
#include "helper/magic_enum_wrapper.hpp"

input::ReplayGameInput::ReplayGameInput(std::shared_ptr<recorder::RecordingReader> recording_reader)
    : GameInput{ GameInputType::Recording },
      m_recording_reader{ std::move(recording_reader) } { }

void input::ReplayGameInput::update(const SimulationStep simulation_step_index) {
    while (true) {
        if (is_end_of_recording()) {
            break;
        }

        const auto& record = m_recording_reader->at(m_next_record_index);

        if (record.tetrion_index != m_target_tetrion->tetrion_index()) {
            // the current record is not for this tetrion => discard record and keep reading
            ++m_next_record_index;
            continue;
        }

        const auto is_record_for_current_step = (record.simulation_step_index == simulation_step_index);

        if (not is_record_for_current_step) {
            break;
        }

        spdlog::debug("replaying event {} at step {}", magic_enum::enum_name(record.event), simulation_step_index);

        GameInput::handle_event(record.event, simulation_step_index);

        ++m_next_record_index;
    }

    GameInput::update(simulation_step_index);
}

void input::ReplayGameInput::late_update(const SimulationStep simulation_step_index) {
    GameInput::late_update(simulation_step_index);

    while (true) {
        if (m_next_snapshot_index >= m_recording_reader->snapshots().size()) {
            break;
        }

        const auto& snapshot = m_recording_reader->snapshots().at(m_next_snapshot_index);
        if (snapshot.tetrion_index() != m_target_tetrion->tetrion_index()) {
            ++m_next_snapshot_index;
            continue;
        }

        // the snapshot corresponds to this tetrion
        assert(snapshot.tetrion_index() == m_target_tetrion->tetrion_index());

        if (snapshot.simulation_step_index() != simulation_step_index) {
            break;
        }

        // create a snapshot from the current state of the tetrion and compare it to the loaded snapshot
        const auto current_snapshot = TetrionSnapshot{ m_target_tetrion->core_information(), simulation_step_index };


        spdlog::info("comparing tetrion snapshots at simulation_step {}", simulation_step_index);

        const auto compare_result = current_snapshot.compare_to(snapshot);
        if (compare_result.has_value()) {
            spdlog::info("snapshots are equal");
        } else {
            spdlog::error("{}", compare_result.error());
            throw std::runtime_error{ "snapshots are not equal" };
        }

        ++m_next_snapshot_index;
    }
}

[[nodiscard]] bool input::ReplayGameInput::is_end_of_recording() const {
    return m_next_record_index >= m_recording_reader->num_records();
}