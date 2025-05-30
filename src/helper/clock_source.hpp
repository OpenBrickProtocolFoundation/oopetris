#pragma once


#include <core/helper/types.hpp>

#include "./export_symbols.hpp"

#include <optional>
#include <stdexcept>

struct ClockSource {
    OOPETRIS_GRAPHICS_EXPORTED virtual ~ClockSource() = default;

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual SimulationStep simulation_step_index() const = 0;
    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual bool can_be_paused() = 0;

    OOPETRIS_GRAPHICS_EXPORTED virtual void pause() {
        throw std::runtime_error("not implemented");
    }

    /**
     * @brief Resumes the clock.
     * @return The duration of the pause.
     */
    OOPETRIS_GRAPHICS_EXPORTED virtual double resume() {
        throw std::runtime_error("not implemented");
    };
};

struct LocalClock : public ClockSource {
private:
    double m_start_time;
    double m_step_duration;
    std::optional<double> m_paused_at;

public:
    OOPETRIS_GRAPHICS_EXPORTED explicit LocalClock(u32 target_frequency);
    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED SimulationStep simulation_step_index() const override;
    OOPETRIS_GRAPHICS_EXPORTED bool can_be_paused() override;
    OOPETRIS_GRAPHICS_EXPORTED void pause() override;
    OOPETRIS_GRAPHICS_EXPORTED double resume() override;
};
