#pragma once

#include "./windows.hpp"
#include <core/game/mino_stack.hpp>
#include <core/helper/expected.hpp>
#include <core/helper/utils.hpp>

#include "./tetrion_core_information.hpp"

#include <memory>
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

    OOPETRIS_RECORDINGS_EXPORTED TetrionSnapshot(
            u8 tetrion_index,
            Level level,
            Score score,
            LineCount lines_cleared,
            SimulationStep simulation_step_index,
            MinoStack mino_stack
    );

    OOPETRIS_RECORDINGS_EXPORTED static helper::expected<TetrionSnapshot, std::string> from_istream(
            std::istream& istream
    );

    OOPETRIS_RECORDINGS_EXPORTED
    TetrionSnapshot(std::unique_ptr<TetrionCoreInformation> information, SimulationStep simulation_step_index);

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] u8 tetrion_index() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] Level level() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] Score score() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] LineCount lines_cleared() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] u64 simulation_step_index() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] const MinoStack& mino_stack() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] std::vector<char> to_bytes() const;

    OOPETRIS_RECORDINGS_EXPORTED [[nodiscard]] helper::expected<void, std::string> compare_to(
            const TetrionSnapshot& other
    ) const;
};
