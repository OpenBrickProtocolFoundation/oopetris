#pragma once

#include "lua_handler.hpp"
#include "platform/input.hpp"

#include <memory>

struct SolAIInput : public Input {
private:
    external::LUAHandler m_handler;

public:
    SolAIInput(const std::filesystem::path& file);

    void update(SimulationStep simulation_step_index) override;
    void late_update(SimulationStep simulation_step_index) override;
};
