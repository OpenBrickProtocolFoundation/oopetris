#pragma once

#include "mino.hpp"
#include "manager/service_provider.hpp"
#include "helper/types.hpp"
#include <algorithm>
#include <magic_enum.hpp>
#include <vector>

struct Grid;

struct MinoStack final {
private:
    std::vector<Mino> m_minos;

public:
    void clear_row_and_let_sink(int row);
    [[nodiscard]] bool is_empty(Point coordinates) const;
    void set(Point coordinates, TetrominoType type);
    void draw_minos(const ServiceProvider& service_provider, const Grid* grid) const;

    [[nodiscard]] usize num_minos() const {
        return m_minos.size();
    }

    [[nodiscard]] const std::vector<Mino>& minos() const {
        return m_minos;
    }

    [[nodiscard]] bool operator==(const MinoStack& other) const {
        if (m_minos.size() != other.m_minos.size()) {
            return false;
        }

        const auto all_of_this_in_other = std::all_of(m_minos.cbegin(), m_minos.cend(), [&](const auto& mino) {
            return std::find(other.m_minos.cbegin(), other.m_minos.cend(), mino) != end(other.m_minos);
        });

        if (not all_of_this_in_other) {
            return false;
        }

        const auto all_of_other_in_this =
                std::all_of(other.m_minos.cbegin(), other.m_minos.cend(), [this](const auto& mino) {
                    return std::find(m_minos.cbegin(), m_minos.cend(), mino) != end(m_minos);
                });

        return all_of_other_in_this;
    }

    [[nodiscard]] bool operator!=(const MinoStack& other) const {
        return not(*this == other);
    }
};

std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack);
