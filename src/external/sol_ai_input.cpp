#include "sol_ai_input.hpp"
#include "game/tetrion.hpp"


SolAIInput::SolAIInput(const std::filesystem::path& file) : Input{ InputType::AI } {


    const auto loaded = external::load_from_file(file);

    if (not loaded.has_value()) {
        throw std::runtime_error(fmt::format("Coudln't load lua file with error: {}", loaded.error()));
    }
    m_handler = loaded.value()
}

void SolAIInput::update(const SimulationStep simulation_step_index) {

    m_handler.update(simulation_step_index, m_target_tetrion);
}

void SolAIInput::late_update(const SimulationStep simulation_step_index) {

    const auto current_snapshot = TetrionSnapshot{ m_target_tetrion->core_information(), simulation_step_index };

    m_handler.late_update(simulation_step_index, current_snapshot);
}
