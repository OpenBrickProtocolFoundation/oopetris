#include "grid.hpp"

Grid::Grid(Point offset, int tile_size) : m_offset{ offset - Point{ 0, invisible_rows * tile_size }}, m_tile_size{ tile_size } {
}

[[nodiscard]] Point Grid::tile_size() const {
    return Point{ m_tile_size, m_tile_size };
}

[[nodiscard]] Point Grid::to_screen_coords(Point grid_coords) const {
    return m_offset + grid_coords * m_tile_size;
}

void Grid::render(const Application& app) const {
    draw_preview_background(app);
    draw_hold_background(app);
    draw_playing_field_background(app);
}

void Grid::draw_preview_background(const Application& app) const {
    draw_background(
            app,
            Rect{
                    preview_background_position,
                    preview_background_position + preview_extends - Point{1, 1},
    }
    );
}

void Grid::draw_hold_background(const Application& app) const {
    draw_background(
            app,
            Rect{
                    hold_background_position,
                    hold_background_position + hold_background_extends - Point{1, 1},
    }
    );
}

void Grid::draw_playing_field_background(const Application& app) const {
    draw_background(
            app,
            Rect{
                    Point::zero(),
                    Point{width - 1, height - invisible_rows - 1},
    }
    );
}
void Grid::draw_background(const Application& app, Rect grid_rect) const {
    const auto top_left = m_offset + Point{ 0, m_tile_size * invisible_rows } + grid_rect.top_left * m_tile_size;
    const auto bottom_right = top_left + Point{ grid_rect.width(), grid_rect.height() } * m_tile_size;
    app.renderer().draw_rect_filled(Rect{ top_left, bottom_right }, background_color);

    for (int column = 0; column <= grid_rect.width(); ++column) {
        const auto start = top_left + Point{ column * m_tile_size, 0 };
        const auto end = Point{ start.x, start.y + grid_rect.height() * m_tile_size };
        app.renderer().draw_line(start, end, grid_color);
        app.renderer().draw_line(start - Point{ 1, 0 }, end - Point{ 1, 0 }, grid_color);
    }

    for (int row = 0; row <= grid_rect.height(); ++row) {
        const auto start = top_left + Point{ 0, row * m_tile_size };
        const auto end = Point{ bottom_right.x, start.y };
        app.renderer().draw_line(start, end, grid_color);
        app.renderer().draw_line(start - Point{ 0, 1 }, end - Point{ 0, 1 }, grid_color);
    }

    const auto outline_top_left = top_left - Point{ 2, 2 };
    const auto outline_bottom_right = Point{
        top_left.x + grid_rect.width() * m_tile_size + 1,
        top_left.y + grid_rect.height() * m_tile_size + 1,
    };

    const Rect outline_rect = Rect{
        outline_top_left,
        outline_bottom_right,
    };
    app.renderer().draw_rect_outline(outline_rect, border_color);
}
