#pragma once

#include "./export_symbols.hpp"
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

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED u8 tetrion_index() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED Level level() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED Score score() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED LineCount lines_cleared() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED u64 simulation_step_index() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED const MinoStack& mino_stack() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED std::vector<char> to_bytes() const;

    [[nodiscard]] OOPETRIS_RECORDINGS_EXPORTED helper::expected<void, std::string> compare_to(
            const TetrionSnapshot& other
    ) const;
};
