#pragma once

#include "helper/optional.hpp"
#include "helper/types.hpp"

struct ClockSource {
    virtual ~ClockSource() = default;

    [[nodiscard]] virtual SimulationStep simulation_step_index() const = 0;
    [[nodiscard]] virtual bool can_be_paused() = 0;

    virtual void pause() {
        throw std::exception{};
    }

    /**
     * @brief Resumes the clock.
     * @return The duration of the pause.
     */
    virtual double resume() {
        throw std::exception{};
    };
};

struct LocalClock : public ClockSource {
private:
    double m_start_time;
    double m_step_duration;
    helpers::optional<double> m_paused_at{};

public:
    explicit LocalClock(u32 target_frequency);
    [[nodiscard]] SimulationStep simulation_step_index() const override;
    bool can_be_paused() override;
    void pause() override;
    double resume() override;
};
