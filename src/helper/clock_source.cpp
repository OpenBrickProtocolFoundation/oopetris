#include "helper/clock_source.hpp"

#include <SDL.h>
#include <cassert>
#include <spdlog/spdlog.h>

namespace {
    [[nodiscard]] double elapsed_time() {
        return static_cast<double>(SDL_GetTicks64()) / 1000.0;
    }

} // namespace


LocalClock::LocalClock(const u32 target_frequency)
    : m_start_time{ elapsed_time() },
      m_step_duration{ 1.0 / static_cast<double>(target_frequency) } {
    assert(target_frequency >= 1);
}

[[nodiscard]] SimulationStep LocalClock::simulation_step_index() const {
    const double time_since_start = elapsed_time() - m_start_time;
    return static_cast<SimulationStep>(time_since_start / m_step_duration);
}

bool LocalClock::can_be_paused() {
    return true;
}

void LocalClock::pause() {
    if (m_paused_at) {
        throw std::runtime_error("cannot pause if already paused");
    }
    m_paused_at = elapsed_time();
    spdlog::info("pausing clock");
}

double LocalClock::resume() {
    if (not m_paused_at) {
        throw std::runtime_error("must have been paused before to be able to resume");
    }
    const auto duration = elapsed_time() - *m_paused_at;
    m_start_time += duration;
    m_paused_at = std::nullopt;
    spdlog::info("resuming clock (duration of pause: {} s)", duration);
    return duration;
}
