#include "clock_source.hpp"
#include "application.hpp"
#include <cassert>

LocalClock::LocalClock(const u32 target_frequency)
    : m_start_time{ Application::elapsed_time() },
      m_step_duration{ 1.0 / static_cast<double>(target_frequency) } {
    assert(target_frequency >= 1);
}

[[nodiscard]] SimulationStep LocalClock::simulation_step_index() const {
    const double time_since_start = Application::elapsed_time() - m_start_time;
    return static_cast<SimulationStep>(time_since_start / m_step_duration);
}
