#pragma once

#include "application.hpp"
#include "tetrion.hpp"
#include "utils.hpp"
#include <array>
#include <concepts>
#include <cstdlib>
#include <span>
#include <vector>

struct TetrionSnapshot final {
public:
    using Level = decltype(Tetrion::m_level);
    using Score = decltype(Tetrion::m_score);
    using LineCount = decltype(Tetrion::m_lines_cleared);
    using SimulationStepIndex = decltype(Application::simulation_step_index());

private:
    u8 m_tetrion_index;
    Level m_level;
    Score m_score;
    LineCount m_lines_cleared;
    SimulationStepIndex m_simulation_step_index;
    MinoStack m_mino_stack;

public:
    using MinoCount = u64;
    using Coordinate = i32;
    using MinoType = u8;

    TetrionSnapshot(
            u8 tetrion_index,
            Level level,
            Score score,
            LineCount lines_cleared,
            SimulationStepIndex simulation_step_index,
            MinoStack mino_stack
    );

    explicit TetrionSnapshot(std::istream& istream);

    explicit TetrionSnapshot(const Tetrion& tetrion);

    [[nodiscard]] auto tetrion_index() const {
        return m_tetrion_index;
    }

    [[nodiscard]] auto simulation_step_index() const {
        return m_simulation_step_index;
    }

    [[nodiscard]] std::vector<char> to_bytes() const;

    bool compare_to(const TetrionSnapshot& other, bool log_result) const;

private:
    template<std::integral Integral>
    static void append(std::vector<char>& vector, const Integral value) {
        const auto little_endian_value = utils::to_little_endian(value);
        const char* const start = reinterpret_cast<const char*>(&little_endian_value);
        const char* const end = start + sizeof(little_endian_value);
        for (const char* pointer = start; pointer < end; ++pointer) {
            vector.push_back(*pointer);
        }
    }

    template<std::integral Integral>
    [[nodiscard]] static tl::optional<Integral> read_from_istream(std::istream& istream) {
        if (not istream) {
            return tl::nullopt;
        }
        auto value = Integral{};
        istream.read(reinterpret_cast<char*>(&value), sizeof(Integral));
        if (not istream) {
            return tl::nullopt;
        }
        return utils::from_little_endian(value);
    }
};
