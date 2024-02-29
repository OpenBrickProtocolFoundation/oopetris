#pragma once

#include "helper/clock_source.hpp"
#include "input_creator.hpp"
#include "manager/recording/recording.hpp"
#include "ui/widget.hpp"

struct Game : public ui::Widget {
private:
    using TetrionHeaders = std::vector<recorder::TetrionHeader>;

    std::unique_ptr<ClockSource> m_clock_source;
    SimulationStep m_simulation_step_index{ 0 };
    std::unique_ptr<Tetrion> m_tetrion;
    std::unique_ptr<Input> m_input;
    bool m_is_paused{ false };

public:
    explicit Game(
            ServiceProvider* const service_provider,
            const ui::Layout& layout,
            std::unique_ptr<Input>&& input,
            const tetrion::StartingParameters& starting_parameters
    );

    void update() override;
    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] helper::BoolWrapper<ui::EventHandleType> handle_event(const SDL_Event& event, const Window* window)
            override;

    void set_paused(bool paused);
    [[nodiscard]] bool is_paused() const;

    [[nodiscard]] bool is_game_over() const;
};