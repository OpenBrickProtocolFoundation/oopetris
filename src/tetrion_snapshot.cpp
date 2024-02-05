#include "tetrion_snapshot.hpp"
#include <spdlog/spdlog.h>
#include <sstream>
#include <string_view>

TetrionSnapshot::TetrionSnapshot(
        const u8 tetrion_index, // NOLINT(bugprone-easily-swappable-parameters)
        const Level level,
        const Score score,
        const LineCount lines_cleared,
        const SimulationStep simulation_step_index,
        MinoStack mino_stack
)
    : m_tetrion_index{ tetrion_index },
      m_level{ level },
      m_score{ score },
      m_lines_cleared{ lines_cleared },
      m_simulation_step_index{ simulation_step_index },
      m_mino_stack{ std::move(mino_stack) } { }

TetrionSnapshot::TetrionSnapshot(std::istream& istream) {
    const auto tetrion_index = read_from_istream<u8>(istream);
    if (not tetrion_index.has_value()) {
        spdlog::error("unable to read tetrion index from snapshot");
        throw std::exception{};
    }
    m_tetrion_index = *tetrion_index;

    const auto level = read_from_istream<Level>(istream);
    if (not level.has_value()) {
        spdlog::error("unable to read level from snapshot");
        throw std::exception{};
    }
    m_level = *level;

    const auto score = read_from_istream<Score>(istream);
    if (not score.has_value()) {
        spdlog::error("unable to read score from snapshot");
        throw std::exception{};
    }
    m_score = *score;

    const auto lines_cleared = read_from_istream<LineCount>(istream);
    if (not lines_cleared.has_value()) {
        spdlog::error("unable to read lines cleared from snapshot");
        throw std::exception{};
    }
    m_lines_cleared = *lines_cleared;

    const auto simulation_step_index = read_from_istream<SimulationStep>(istream);
    if (not simulation_step_index.has_value()) {
        spdlog::error("unable to read simulation step index from snapshot");
        throw std::exception{};
    }
    m_simulation_step_index = *simulation_step_index;

    const auto num_minos = read_from_istream<MinoCount>(istream);
    if (not num_minos.has_value()) {
        spdlog::error("unable to read number of minos from snapshot");
        throw std::exception{};
    }

    for (MinoCount i = 0; i < *num_minos; ++i) {
        const auto x = read_from_istream<Coordinate>(istream);
        if (not x.has_value()) {
            spdlog::error("unable to read x coordinate of mino from snapshot");
            throw std::exception{};
        }

        const auto y = read_from_istream<Coordinate>(istream);
        if (not y.has_value()) {
            spdlog::error("unable to read y coordinate of mino from snapshot");
            throw std::exception{};
        }

        const auto type = read_from_istream<MinoType>(istream);
        if (not type.has_value()) {
            spdlog::error("unable to read tetromino type of mino from snapshot");
            throw std::exception{};
        }

        using PointCoordinate = decltype(Point::x);

        m_mino_stack.set(
                Point{ static_cast<PointCoordinate>(*x), static_cast<PointCoordinate>(*y) },
                static_cast<TetrominoType>(*type)
        );
    }
}

TetrionSnapshot::TetrionSnapshot(const Tetrion& tetrion, const SimulationStep simulation_step_index)
    : TetrionSnapshot{ tetrion.tetrion_index(), tetrion.level(),       tetrion.score(),
                       tetrion.lines_cleared(), simulation_step_index, tetrion.mino_stack() } { }

[[nodiscard]] std::vector<char> TetrionSnapshot::to_bytes() const {
    auto bytes = std::vector<char>{};
    append(bytes, m_tetrion_index);
    append(bytes, m_level);
    append(bytes, m_score);
    append(bytes, m_lines_cleared);
    append(bytes, m_simulation_step_index);
    const auto num_minos = static_cast<u64>(m_mino_stack.num_minos());
    append(bytes, static_cast<MinoCount>(num_minos));
    for (const auto& mino : m_mino_stack.minos()) {
        append(bytes, static_cast<Coordinate>(mino.position().x));
        append(bytes, static_cast<Coordinate>(mino.position().y));
        append(bytes, static_cast<MinoType>(mino.type()));
    }
    return bytes;
}


namespace {

    template<typename Value>
    void compare_values(
            const std::string_view name,
            const Value& this_value,
            const Value& other_value,
            const bool log_result,
            bool& result
    ) {
        if (this_value != other_value) {
            if (log_result) {
                spdlog::error("{} do not match ({} vs. {})", name, this_value, other_value);
            }
            result = false;
        }
    }
} // namespace

bool TetrionSnapshot::compare_to(const TetrionSnapshot& other, const bool log_result) const {
    bool snapshots_are_equal = true;

    compare_values("tetrion indices", m_tetrion_index, other.m_tetrion_index, log_result, snapshots_are_equal);
    compare_values("levels", m_level, other.m_level, log_result, snapshots_are_equal);
    compare_values("scores", m_score, other.m_score, log_result, snapshots_are_equal);
    compare_values("numbers of lines cleared", m_lines_cleared, other.m_lines_cleared, log_result, snapshots_are_equal);
    compare_values(
            "simulation step indices", m_simulation_step_index, other.m_simulation_step_index, log_result,
            snapshots_are_equal
    );

    const auto mino_stacks_are_equal = (m_mino_stack == other.m_mino_stack);
    if (mino_stacks_are_equal) {
        if (log_result) {
            std::stringstream ss;
            ss << m_mino_stack;
        }
    } else {
        if (log_result) {
            std::stringstream ss;
            ss << m_mino_stack << " vs. " << other.m_mino_stack;
            spdlog::error("mino stacks do not match ({})", ss.str());
        }
        snapshots_are_equal = false;
    }

    return snapshots_are_equal;
}
