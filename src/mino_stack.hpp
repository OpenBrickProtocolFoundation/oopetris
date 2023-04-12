#pragma once

#include "mino.hpp"
#include "types.hpp"
#include <algorithm>
#include <iostream>
#include <magic_enum.hpp>
#include <vector>
#if defined(__ANDROID__)
#include "ranges.hpp"
#include <algorithm>
#else
#include <ranges>
#endif

struct Grid;

struct MinoStack final {
private:
    std::vector<Mino> m_minos;

public:
    void clear_row_and_let_sink(int row);
    [[nodiscard]] bool is_empty(Point coordinates) const;
    void set(Point coordinates, TetrominoType type);
    void draw_minos(const Application& app, const Grid& grid) const;

    [[nodiscard]] usize num_minos() const {
        return m_minos.size();
    }

    [[nodiscard]] const std::vector<Mino>& minos() const {
        return m_minos;
    }

    [[nodiscard]] bool operator==(const MinoStack& other) const {
        using std::ranges::all_of, std::ranges::find, std::ranges::end;

        if (m_minos.size() != other.m_minos.size()) {
            return false;
        }


#if defined(__ANDROID__)
        const auto all_of_this_in_other = std::all_of(m_minos.cbegin(), m_minos.cend(), [&](const auto& mino) {
            return std::find(other.m_minos.cbegin(), other.m_minos.cend(), mino) != end(other.m_minos);
        });
#else
        const auto all_of_this_in_other =
                all_of(m_minos, [&](const auto& mino) { return find(other.m_minos, mino) != end(other.m_minos); });
#endif

        if (not all_of_this_in_other) {
            return false;
        }


#if defined(__ANDROID__)
        const auto all_of_other_in_this =
                std::all_of(other.m_minos.cbegin(), other.m_minos.cend(), [&](const auto& mino) {
                    return std::find(m_minos.cbegin(), m_minos.cend(), mino) != end(m_minos);
                });


#else
        const auto all_of_other_in_this =
                all_of(other.m_minos, [&](const auto& mino) { return find(m_minos, mino) != end(m_minos); });

#endif

        return all_of_other_in_this;
    }

    [[nodiscard]] bool operator!=(const MinoStack& other) const {
        return not(*this == other);
    }
};

std::ostream& operator<<(std::ostream& os, const MinoStack& mino_stack);
