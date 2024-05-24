#pragma once

#include "game/mino_stack.hpp"
#include "helper/expected.hpp"
#include "helper/utils.hpp"
#include "tetrion_core_information.hpp"

#include <array>
#include <memory>
#include <span>
#include <vector>

struct TetrionSnapshot final {
public:
    using Level = u32;
    using Score = u64;
    using LineCount = u32;

private:
    u8 m_tetrion_index;
    Level m_level;
    Score m_score;
    LineCount m_lines_cleared;
    SimulationStep m_simulation_step_index;
    MinoStack m_mino_stack;

    explicit TetrionSnapshot(std::istream& istream);

public:
    using MinoCount = u64;
    using Coordinate = u8;

    TetrionSnapshot(
            u8 tetrion_index,
            Level level,
            Score score,
            LineCount lines_cleared,
            SimulationStep simulation_step_index,
            MinoStack mino_stack
    );

    static helper::expected<TetrionSnapshot, std::string> from_istream(std::istream& istream);

    TetrionSnapshot(std::unique_ptr<TetrionCoreInformation> information, SimulationStep simulation_step_index);

    [[nodiscard]] u8 tetrion_index() const;

    [[nodiscard]] Level level() const;

    [[nodiscard]] Score score() const;

    [[nodiscard]] LineCount lines_cleared() const;

    [[nodiscard]] u64 simulation_step_index() const;

    [[nodiscard]] const MinoStack& mino_stack() const;

    [[nodiscard]] std::vector<char> to_bytes() const;

    [[nodiscard]] helper::expected<bool, std::string> compare_to(const TetrionSnapshot& other) const;
};
