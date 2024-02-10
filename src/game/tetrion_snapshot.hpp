#pragma once

#include "tetrion.hpp"
#include "utils.hpp"
#include <array>
#include <span>
#include <vector>

struct TetrionSnapshot final {
public:
    using Level = decltype(Tetrion::m_level);
    using Score = decltype(Tetrion::m_score);
    using LineCount = decltype(Tetrion::m_lines_cleared);

private:
    u8 m_tetrion_index;
    Level m_level;
    Score m_score;
    LineCount m_lines_cleared;
    SimulationStep m_simulation_step_index;
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
            SimulationStep simulation_step_index,
            MinoStack mino_stack
    );

    explicit TetrionSnapshot(std::istream& istream);

    TetrionSnapshot(const Tetrion& tetrion, SimulationStep simulation_step_index);

    [[nodiscard]] auto tetrion_index() const {
        return m_tetrion_index;
    }

    [[nodiscard]] auto simulation_step_index() const {
        return m_simulation_step_index;
    }

    [[nodiscard]] std::vector<char> to_bytes() const;

    [[nodiscard]] bool compare_to(const TetrionSnapshot& other, bool log_result) const;

private:
    template<utils::integral Integral>
    static void append(std::vector<char>& vector, const Integral value) {
        const auto little_endian_value = utils::to_little_endian(value);
        const char* const start = reinterpret_cast<const char*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                &little_endian_value
        );
        const char* const end =
                start + sizeof(little_endian_value); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        for (const char* pointer = start; pointer < end;
             ++pointer) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            vector.push_back(*pointer);
        }
    }

    template<utils::integral Integral>
    [[nodiscard]] static tl::optional<Integral> read_from_istream(std::istream& istream) {
        if (not istream) {
            return tl::nullopt;
        }
        auto value = Integral{};
        istream.read(
                reinterpret_cast<char*>(&value), sizeof(Integral) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        );
        if (not istream) {
            return tl::nullopt;
        }
        return utils::from_little_endian(value);
    }
};
