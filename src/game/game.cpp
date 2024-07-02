
#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>

#include "game.hpp"
#include "input/replay_input.hpp"

Game::Game(
        ServiceProvider* const service_provider,
        const std::shared_ptr<input::GameInput>& input,
        const tetrion::StartingParameters& starting_parameters,
        u32 simulation_frequency,
        const ui::Layout& layout,
        bool is_top_level
)
    : ui::Widget{ layout, ui::WidgetType::Component, is_top_level },
      m_clock_source{ std::make_unique<LocalClock>(simulation_frequency) },
      m_input{ input } {


    spdlog::info("starting level for tetrion {}", starting_parameters.starting_level);

    m_tetrion = std::make_unique<Tetrion>(
            starting_parameters.tetrion_index, starting_parameters.seed, starting_parameters.starting_level,
            service_provider, starting_parameters.recording_writer, layout, false
    );

    m_tetrion->spawn_next_tetromino(0);

    m_input->set_target_tetrion(m_tetrion.get());
    if (starting_parameters.recording_writer.has_value()) {
        const auto recording_writer = starting_parameters.recording_writer.value();
        const auto tetrion_index = starting_parameters.tetrion_index;
        m_input->set_event_callback([recording_writer,
                                     tetrion_index](InputEvent event, SimulationStep simulation_step_index) {
            spdlog::debug("event: {} (step {})", magic_enum::enum_name(event), simulation_step_index);

            recording_writer->add_record(tetrion_index, simulation_step_index, event);
        });
    }
}

void Game::update() {
    if (is_game_finished()) {
        return;
    }

    if (m_is_paused) {
        // if we would still be in pause mode, update() wouldn't have been called in the first place => we
        // must resume from pause
        set_paused(false);
    }

    while (m_simulation_step_index < m_clock_source->simulation_step_index()) {
        ++m_simulation_step_index;
        m_input->update(m_simulation_step_index);
        m_tetrion->update_step(m_simulation_step_index);
        m_input->late_update(m_simulation_step_index);
    }
}

void Game::render(const ServiceProvider& service_provider) const {
    m_tetrion->render(service_provider);
}

[[nodiscard]] helper::BoolWrapper<std::pair<ui::EventHandleType, ui::Widget*>>
Game::handle_event(const std::shared_ptr<input::InputManager>& /*input_manager*/, const SDL_Event& /*event*/) {
    return false;
}

void Game::set_paused(bool paused) {
    m_is_paused = paused;

    assert(m_clock_source->can_be_paused());
    if (paused) {
        m_clock_source->pause();
    } else {
        m_clock_source->resume();
    }

    auto listener = utils::is_child_class<EventListener>(m_input);

    if (listener.has_value()) {
        listener.value()->set_paused(paused);
    }
}

[[nodiscard]] bool Game::is_paused() const {
    return m_is_paused;
}


[[nodiscard]] bool Game::is_game_finished() const {
    if (m_tetrion->is_game_over()) {
        return true;
    };

    const auto input_as_replay = utils::is_child_class<input::ReplayGameInput>(m_input);
    if (input_as_replay.has_value()) {
        return input_as_replay.value()->is_end_of_recording();
    }

    return false;
}


[[nodiscard]] const std::shared_ptr<input::GameInput>& Game::game_input() const {
    return m_input;
}
