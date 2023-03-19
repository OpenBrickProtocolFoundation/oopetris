#include "mino.hpp"
#include "application.hpp"
#include "grid.hpp"

void Mino::render(const Application& app, const Grid& grid) const {
    const Color foreground = get_foreground_color(m_type);
    const Color background = get_background_color(m_type);
    const Point top_left = grid.to_screen_coords(m_position);
    const Point top_right = top_left + Point{ grid.tile_size().x - 1, 0 };
    const Point bottom_left = top_left + Point{ 0, grid.tile_size().y - 1 };
    const Point bottom_right = top_left + grid.tile_size() - Point{ 1, 1 };
    app.renderer().draw_rect(Rect{ top_left, bottom_right }, background);

    const Point inner_top_left = top_left + Point{ inset, inset };
    const Point inner_top_right = top_right + Point{ -inset, inset };
    const Point inner_bottom_left = bottom_left + Point{ inset, -inset };
    const Point inner_bottom_right = bottom_right - Point{ inset, inset };
    app.renderer().draw_line(top_left, inner_top_left, Color::white(140));
    app.renderer().draw_line(bottom_left, inner_bottom_left, Color::white(100));
    app.renderer().draw_line(top_right, inner_top_right, Color{ 80, 80, 80, 255 });
    app.renderer().draw_line(bottom_right, inner_bottom_right, Color{ 80, 80, 80, 180 });

    app.renderer().draw_rect(Rect{ inner_top_left, inner_bottom_right }, foreground);
}
