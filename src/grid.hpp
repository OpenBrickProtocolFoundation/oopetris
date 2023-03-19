#pragma once

#include "point.hpp"

struct Grid final {
private:
    Point m_offset;
    int m_tile_size;

public:
    Grid(Point offset, int tile_size) : m_offset{ offset }, m_tile_size{ tile_size } { }

    Point tile_size() const {
        return Point{ m_tile_size, m_tile_size };
    }

    Point to_screen_coords(Point grid_coords) const {
        return m_offset + grid_coords * m_tile_size;
    }
};
