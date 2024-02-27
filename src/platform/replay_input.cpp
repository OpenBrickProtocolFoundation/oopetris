#include "replay_input.hpp"


ReplayInput::ReplayInput(std::shared_ptr<RecordingReader> recording_reader)
    : Input{ InputType::Recording },
      m_recording_reader{ recording_reader } { }

void ReplayInput::update(const SimulationStep simulation_step_index) {
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

        Input::handle_event(record.event, simulation_step_index);

        ++m_next_record_index;
    }

    Input::update(simulation_step_index);
}

void ReplayInput::late_update(const SimulationStep simulation_step_index) {
    Input::late_update(simulation_step_index);

    while (true) {
        if (m_next_snapshot_index >= m_recording_reader->m_snapshots.size()) {
            break;
        }

        const auto& snapshot = m_recording_reader->m_snapshots.at(m_next_snapshot_index);
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
        const auto current_snapshot = TetrionSnapshot{ *m_target_tetrion, simulation_step_index };
#ifdef DEBUG_BUILD
        static constexpr auto verbose_logging = true;
#else
        static constexpr auto verbose_logging = false;
#endif
        if constexpr (verbose_logging) {
            spdlog::info("comparing tetrion snapshots");
        }
        const auto snapshots_are_equal = current_snapshot.compare_to(snapshot, verbose_logging);
        if (snapshots_are_equal) {
            if constexpr (verbose_logging) {
                spdlog::info("snapshots are equal");
            }
        } else {
            spdlog::error("snapshots are not equal");
            throw std::exception{};
        }
        ++m_next_snapshot_index;
    }
}

[[nodiscard]] bool ReplayInput::is_end_of_recording() const {
    return m_next_record_index >= m_recording_reader->num_records();
}
