#include "grid.hpp"
#include "rect.hpp"

Grid::Grid(Point offset, int tile_size) :  m_start_point{ offset }, m_offset{ offset - Point{ 0, invisible_rows * tile_size }}, m_tile_size{ tile_size } {
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
    const Point bottom_right{
        width * m_tile_size - 1,
        (height - invisible_rows) * m_tile_size,
    };
    app.renderer().draw_rect_filled(m_start_point + Rect{ Point::zero(), bottom_right }, background_color);
    app.renderer().draw_line(
            m_start_point + Point{ bottom_right.x + 1, 0 },
            m_start_point + Point{ bottom_right.x + 1, app.window().size().y - 1 }, border_color
    );
}
