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

    [[nodiscard]] u32 num_minos() const;

    [[nodiscard]] const std::vector<Mino>& minos() const;

    [[nodiscard]] bool operator==(const MinoStack& other) const;

    [[nodiscard]] bool operator!=(const MinoStack& other) const;
};

std::ostream& operator<<(std::ostream& ostream, const MinoStack& mino_stack);
