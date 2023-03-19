#pragma once

#include "application.hpp"
#include "mino.hpp"
#include "point.hpp"
#include <algorithm>
#include <vector>

struct Grid final {
public:
    static constexpr int width = 10;
    static constexpr int height = 22;
    static constexpr int invisible_rows = 2;
    static constexpr int num_tiles = width * height;

private:
    Point m_offset;
    int m_tile_size;
    std::vector<Mino> m_minos;

public:
    Grid(Point offset, int tile_size) : m_offset{ offset - Point{ 0, invisible_rows * tile_size }}, m_tile_size{ tile_size } {
        m_minos.reserve(num_tiles);
    }

    Point tile_size() const {
        return Point{ m_tile_size, m_tile_size };
    }

    Point to_screen_coords(Point grid_coords) const {
        return m_offset + grid_coords * m_tile_size;
    }

    void render(const Application& app) const {
        const Point bottom_right{
            width * m_tile_size - 1,
            (height - invisible_rows) * m_tile_size,
        };
        app.renderer().draw_rect(Rect{ Point::zero(), bottom_right }, Color::white());
        for (const Mino& mino : m_minos) {
            if (mino.position().y >= invisible_rows) {
                mino.render(app, *this);
            }
        }
    }

    void set(Point coordinates, TetrominoType type) {
        const Mino to_insert = Mino{ coordinates, type };
        for (Mino& current : m_minos) {
            if (current.position() == coordinates) {
                current = to_insert;
                return;
            }
        }
        m_minos.push_back(to_insert);
    }

    void clear_row_and_let_sink(int row) {
        m_minos.erase(
                std::remove_if(
                        m_minos.begin(), m_minos.end(), [&](const Mino& mino) { return mino.position().y == row; }
                ),
                m_minos.end()
        );
        for (Mino& mino : m_minos) {
            if (mino.position().y < row) {
                ++mino.position().y;
            }
        }
    }

    bool is_empty(Point coordinates) const {
        for (const Mino& mino : m_minos) {
            if (mino.position() == coordinates) {
                return false;
            }
        }
        return true;
    }
};
