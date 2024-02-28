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

public:
    using MinoCount = u64;
    using Coordinate = u8;
    using MinoType = u8;

    TetrionSnapshot(
            u8 tetrion_index,
            Level level,
            Score score,
            LineCount lines_cleared,
            SimulationStep simulation_step_index,
            const MinoStack& mino_stack
    );

    explicit TetrionSnapshot(std::istream& istream);

    TetrionSnapshot(const Tetrion& tetrion, SimulationStep simulation_step_index);

    [[nodiscard]] u8 tetrion_index() const;

    [[nodiscard]] u64 simulation_step_index() const;

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
    [[nodiscard]] static helper::optional<Integral> read_from_istream(std::istream& istream) {
        if (not istream) {
            return helper::nullopt;
        }
        auto value = Integral{};
        istream.read(
                reinterpret_cast<char*>(&value),
                sizeof(Integral) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        );
        if (not istream) {
            return helper::nullopt;
        }
        return utils::from_little_endian(value);
    }
};
