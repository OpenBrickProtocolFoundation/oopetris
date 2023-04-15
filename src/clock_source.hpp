#pragma once

#include "types.hpp"

struct ClockSource {
    virtual ~ClockSource() = default;

    [[nodiscard]] virtual SimulationStep simulation_step_index() const = 0;
};

struct LocalClock : public ClockSource {
private:
    double m_start_time;
    double m_step_duration;

public:
    LocalClock(u32 target_frequency);

    [[nodiscard]] SimulationStep simulation_step_index() const override;
};
