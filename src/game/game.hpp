#pragma once

#include <recordings/utility/recording.hpp>

#include "helper/clock_source.hpp"
#include "input/input_creator.hpp"
#include "tetrion.hpp"
#include "ui/widget.hpp"

struct Game : public ui::Widget {
private:
    using TetrionHeaders = std::vector<recorder::TetrionHeader>;

    std::unique_ptr<ClockSource> m_clock_source;
    SimulationStep m_simulation_step_index{ 0 };
    std::unique_ptr<Tetrion> m_tetrion;
    std::shared_ptr<input::GameInput> m_input;
    bool m_is_paused{ false };

public:
    explicit Game(
            ServiceProvider* service_provider,
            const std::shared_ptr<input::GameInput>& input,
            const tetrion::StartingParameters& starting_parameters,
            u32 simulation_frequency,
            const ui::Layout& layout,
            bool is_top_level
    );

    void update() override;

    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] Widget::EventHandleResult

    handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

    void set_paused(bool paused);
    [[nodiscard]] bool is_paused() const;

    [[nodiscard]] bool is_game_finished() const;

    [[nodiscard]] const std::shared_ptr<input::GameInput>& game_input() const;
};
