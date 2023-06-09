#include "grid.hpp"
#include "renderer.hpp"

Grid::Grid(Point offset, usize tile_size) : m_offset{ offset - Point{ 0, static_cast<int>(invisible_rows * tile_size) }}, m_tile_size{ tile_size } {
}

[[nodiscard]] Point Grid::tile_size() const {
    return Point{ static_cast<int>(m_tile_size), static_cast<int>(m_tile_size) };
}

[[nodiscard]] Point Grid::to_screen_coords(Point grid_coords) const {
    return m_offset + grid_coords * m_tile_size;
}

void Grid::render(const ServiceProvider& service_provider) const {
    draw_preview_background(service_provider);
    draw_hold_background(service_provider);
    draw_playing_field_background(service_provider);
}

void Grid::draw_preview_background(const ServiceProvider& service_provider) const {
    draw_background(
            service_provider,
            Rect{
                    preview_background_position,
                    preview_background_position + preview_extends - Point{1, 1},
    }
    );
}

void Grid::draw_hold_background(const ServiceProvider& service_provider) const {
    draw_background(
            service_provider,
            Rect{
                    hold_background_position,
                    hold_background_position + hold_background_extends - Point{1, 1},
    }
    );
}

void Grid::draw_playing_field_background(const ServiceProvider& service_provider) const {
    draw_background(
            service_provider,
            Rect{
                    Point::zero(),
                    Point{width - 1, height - invisible_rows - 1},
    }
    );
}

void Grid::draw_background(const ServiceProvider& service_provider, Rect grid_rect) const {
    const auto top_left =
            m_offset + Point{ 0, static_cast<int>(m_tile_size * invisible_rows) } + grid_rect.top_left * m_tile_size;
    const auto bottom_right = top_left + Point{ grid_rect.width(), grid_rect.height() } * m_tile_size;
    service_provider.renderer().draw_rect_filled(Rect{ top_left, bottom_right }, background_color);

    for (usize column = 0; column <= static_cast<usize>(grid_rect.width()); ++column) {
        const auto start = top_left + Point{ static_cast<int>(column * m_tile_size), 0 };
        const auto end = Point{ start.x, static_cast<int>(start.y + grid_rect.height() * m_tile_size) };
        service_provider.renderer().draw_line(start, end, grid_color);
        service_provider.renderer().draw_line(start - Point{ 1, 0 }, end - Point{ 1, 0 }, grid_color);
    }

    for (usize row = 0; row <= static_cast<usize>(grid_rect.height()); ++row) {
        const auto start = top_left + Point{ 0, static_cast<int>(row * m_tile_size) };
        const auto end = Point{ bottom_right.x, start.y };
        service_provider.renderer().draw_line(start, end, grid_color);
        service_provider.renderer().draw_line(start - Point{ 0, 1 }, end - Point{ 0, 1 }, grid_color);
    }

    const auto outline_top_left = top_left - Point{ 2, 2 };
    const auto outline_bottom_right = Point{
        static_cast<int>(top_left.x + grid_rect.width() * m_tile_size + 1),
        static_cast<int>(top_left.y + grid_rect.height() * m_tile_size + 1),
    };

    const Rect outline_rect = Rect{
        outline_top_left,
        outline_bottom_right,
    };
    service_provider.renderer().draw_rect_outline(outline_rect, border_color);
}
