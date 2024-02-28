#pragma once

#include "helper/types.hpp"
#include "manager/service_provider.hpp"
#include "mino.hpp"

#include <algorithm>
#include <magic_enum.hpp>
#include <vector>

struct MinoStack final {
private:
    using GridPoint = shapes::AbstractPoint<u8>;
    using ScreenCordsFunction = std::function<shapes::UPoint(const GridPoint&)>;

    std::vector<Mino> m_minos;

public:
    void clear_row_and_let_sink(u8 row);
    [[nodiscard]] bool is_empty(GridPoint coordinates) const;
    void set(GridPoint coordinates, TetrominoType type);
    void draw_minos(
            const ServiceProvider& service_provider,
            const double original_scale,
            const ScreenCordsFunction& to_screen_coords,
            const shapes::UPoint& tile_size
    ) const;

    [[nodiscard]] u32 num_minos() const {
        return static_cast<u32>(m_minos.size());
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
