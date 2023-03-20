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
    static constexpr Point preview_background_position{ width + 1, 1 + invisible_rows };
    static constexpr Point preview_tetromino_position = preview_background_position + Point{ 0, 1 };
    static constexpr Point preview_extends{ 4, 4 };
    static constexpr Color background_color{ 12, 12, 12 };
    static constexpr Color border_color{ 42, 42, 42 };

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

        // background of playing field
        app.renderer().draw_rect(Rect{ Point::zero(), bottom_right }, background_color);
        app.renderer().draw_line(
                Point{ bottom_right.x + 1, 0 }, Point{ bottom_right.x + 1, app.window().size().y - 1 }, border_color
        );

        // background of preview
        const Point preview_top_left = to_screen_coords(preview_background_position);
        const Point preview_bottom_right =
                preview_top_left
                + Point{ tile_size().x * preview_extends.x - 1, tile_size().y * preview_extends.y - 1 };
        app.renderer().draw_rect(Rect{ preview_top_left, preview_bottom_right }, background_color);
        app.renderer().draw_line(
                preview_top_left - Point{ 1, 0 }, Point{ preview_top_left.x - 1, preview_bottom_right.y }, border_color
        );
        app.renderer().draw_line(
                Point{ preview_top_left.x - 1, preview_bottom_right.y + 1 }, preview_bottom_right + Point{ 1, 1 },
                border_color
        );
        app.renderer().draw_line(
                Point{ preview_top_left.x - 1, preview_top_left.y - 1 },
                Point{ preview_bottom_right.x + 1, preview_top_left.y - 1 }, border_color
        );
        app.renderer().draw_line(
                Point{ preview_bottom_right.x + 1, preview_top_left.y },
                Point{ preview_bottom_right.x + 1, preview_bottom_right.y }, border_color
        );

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
