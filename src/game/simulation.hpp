#pragma once

#include <recordings/utility/recording.hpp>

#include "core/helper/expected.hpp"
#include "input/input_creator.hpp"
#include "input/replay_input.hpp"
#include "tetrion.hpp"
#include "ui/widget.hpp"

struct Simulation : public ui::Widget {
private:
    using TetrionHeaders = std::vector<recorder::TetrionHeader>;

    SimulationStep m_simulation_step_index{ 0 };
    std::unique_ptr<Tetrion> m_tetrion;
    std::shared_ptr<input::ReplayGameInput> m_input;

public:
    explicit Simulation(
            ServiceProvider* service_provider,
            const std::shared_ptr<input::ReplayGameInput>& input,
            const tetrion::StartingParameters& starting_parameters
    );


    static helper::expected<Simulation, std::string>
    get_replay_simulation(ServiceProvider* service_provider, std::filesystem::path& recording_path);

    void update() override;

    void render(const ServiceProvider& service_provider) const override;
    [[nodiscard]] Widget::EventHandleResult

    handle_event(const std::shared_ptr<input::InputManager>& input_manager, const SDL_Event& event) override;

    [[nodiscard]] bool is_game_finished() const;
};
