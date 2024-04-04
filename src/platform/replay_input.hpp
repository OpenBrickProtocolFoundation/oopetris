#pragma once

#include "input.hpp"
#include "recordings/recording_reader.hpp"

#include <memory>

struct ReplayInput : public Input {
private:
    std::shared_ptr<recorder::RecordingReader> m_recording_reader;
    usize m_next_record_index{ 0 };
    usize m_next_snapshot_index{ 0 };

public:
    ReplayInput(std::shared_ptr<recorder::RecordingReader> recording_reader);

    void update(SimulationStep simulation_step_index) override;
    void late_update(SimulationStep simulation_step_index) override;

    [[nodiscard]] bool is_end_of_recording() const;
};
