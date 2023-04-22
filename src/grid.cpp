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
    draw_playing_field_background(app);
    draw_preview_background(app);
    draw_hold_background(app);
}

void Grid::draw_preview_background(const Application& app) const {
    draw_small_background(app, preview_background_position);
}

void Grid::draw_hold_background(const Application& app) const {
    draw_small_background(app, hold_background_position);
}

void Grid::draw_small_background(const Application& app, Point position) const {
    const Point top_left = to_screen_coords(position);
    const Point bottom_right =
            top_left + Point{ tile_size().x * preview_extends.x - 1, tile_size().y * preview_extends.y - 1 };
    app.renderer().draw_rect_filled(Rect{ top_left, bottom_right }, background_color);

    const Point outline_top_left = top_left - Point{ 1, 1 };
    const Point outline_bottom_right = bottom_right + Point{ 1, 1 };
    const Rect outline_rect = Rect{ outline_top_left, outline_bottom_right };
    app.renderer().draw_rect_outline(outline_rect, border_color);
}

void Grid::draw_playing_field_background(const Application& app) const {
    static constexpr auto visible_height = height - invisible_rows;

    const auto top_left = m_offset + Point{ 0, m_tile_size * invisible_rows };
    const auto bottom_right = top_left + Point{ width, visible_height } * m_tile_size;
    app.renderer().draw_rect_filled(Rect{ top_left, bottom_right }, background_color);

    for (int column = 0; column <= width; ++column) {
        const auto start = top_left + Point{ column * m_tile_size, 0 };
        const auto end = Point{ start.x, start.y + visible_height * m_tile_size };
        app.renderer().draw_line(start, end, grid_color);
        app.renderer().draw_line(start - Point{ 1, 0 }, end - Point{ 1, 0 }, grid_color);
    }

    for (int row = 0; row <= visible_height; ++row) {
        const auto start = top_left + Point{ 0, row * m_tile_size };
        const auto end = Point{ bottom_right.x, start.y };
        app.renderer().draw_line(start, end, grid_color);
        app.renderer().draw_line(start - Point{ 0, 1 }, end - Point{ 0, 1 }, grid_color);
    }

    const Rect outline_rect = Rect{
        top_left - Point{                                   2,                                             2},
        Point{top_left.x + width * m_tile_size + 1, top_left.y + visible_height * m_tile_size + 1},
    };
    app.renderer().draw_rect_outline(outline_rect, border_color);
}
