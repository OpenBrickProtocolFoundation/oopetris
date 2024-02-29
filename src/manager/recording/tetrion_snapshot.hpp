#pragma once

#include "game/tetrion.hpp"
#include "helper/utils.hpp"
#include <array>
#include <span>
#include <vector>

//TODO: this needs versioning, since now we changed some sizes of some types!!
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
            const MinoStack& mino_stack
    );

    static helper::optional<TetrionSnapshot> from_istream(std::istream& istream);

    TetrionSnapshot(const Tetrion& tetrion, SimulationStep simulation_step_index);

    [[nodiscard]] u8 tetrion_index() const;

    [[nodiscard]] u64 simulation_step_index() const;

    [[nodiscard]] std::vector<char> to_bytes() const;

    [[nodiscard]] bool compare_to(const TetrionSnapshot& other, bool log_result) const;
};
