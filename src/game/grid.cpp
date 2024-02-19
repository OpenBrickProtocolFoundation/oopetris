#include "grid.hpp"
#include "graphics/renderer.hpp"

#include <spdlog/spdlog.h>

Grid::Grid(const ui::Layout& layout) : ui::Widget{ layout } {

    const u32 total_x_tiles = preview_extends.x * 2 + 2 + width_in_tiles;
    const u32 total_y_tiles = height_in_tiles - invisible_rows;

    const u32 tile_size_x = layout.get_rect().width() / total_x_tiles;
    const u32 tile_size_y = layout.get_rect().height() / total_y_tiles;

    m_tile_size = std::min(tile_size_y, tile_size_x);
    spdlog::info(
            "creating grid with a tile size of {}px, {}x{}", m_tile_size, m_tile_size * total_x_tiles,
            m_tile_size * total_y_tiles
    );

    m_fill_rect = ui::get_rectangle_aligned(
            layout, { m_tile_size * total_x_tiles, m_tile_size * total_y_tiles },
            ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );
}

[[nodiscard]] shapes::Point Grid::tile_size() const {
    return shapes::Point{ static_cast<int>(m_tile_size), static_cast<int>(m_tile_size) };
}
[[nodiscard]] double Grid::scale_to_original() const {
    return static_cast<double>(m_tile_size) / static_cast<double>(original_tile_size);
}

[[nodiscard]] shapes::Point Grid::to_screen_coords(shapes::Point grid_coords) const {
    return m_fill_rect.top_left + (grid_coords - shapes::Point{ 0, invisible_rows }) * static_cast<int>(m_tile_size);
}

void Grid::render(const ServiceProvider& service_provider) const {
    draw_preview_background(service_provider);
    draw_hold_background(service_provider);
    draw_playing_field_background(service_provider);
}

[[nodiscard]] bool Grid::handle_event(const SDL_Event&, const Window*) {
    return false;
}

void Grid::draw_preview_background(const ServiceProvider& service_provider) const {
    draw_background(
            service_provider,
            shapes::Rect{
                    preview_background_position,
                    preview_background_position + preview_extends - shapes::Point{1, 1},
    }
    );
}

void Grid::draw_hold_background(const ServiceProvider& service_provider) const {
    draw_background(
            service_provider,
            shapes::Rect{
                    hold_background_position,
                    hold_background_position + hold_background_extends - shapes::Point{1, 1},
    }
    );
}

void Grid::draw_playing_field_background(const ServiceProvider& service_provider) const {
    draw_background(
            service_provider,
            shapes::Rect{
                    grid_position,
                    grid_position + shapes::Point{width_in_tiles - 1, height_in_tiles - invisible_rows - 1},
    }
    );
}

void Grid::draw_background(const ServiceProvider& service_provider, shapes::Rect grid_rect) const {
    const auto top_left = m_fill_rect.top_left + grid_rect.top_left * static_cast<int>(m_tile_size);


    const auto bottom_right =
            top_left + shapes::Point{ grid_rect.width(), grid_rect.height() } * static_cast<int>(m_tile_size);

    service_provider.renderer().draw_rect_filled(shapes::Rect{ top_left, bottom_right }, background_color);

    for (usize column = 0; column <= static_cast<usize>(grid_rect.width()); ++column) {
        const auto start = top_left + shapes::Point{ static_cast<int>(column * m_tile_size), 0 };
        const auto end = shapes::Point{ start.x, static_cast<int>(start.y + grid_rect.height() * m_tile_size) };
        service_provider.renderer().draw_line(start, end, grid_color);
        service_provider.renderer().draw_line(start - shapes::Point{ 1, 0 }, end - shapes::Point{ 1, 0 }, grid_color);
    }

    for (usize row = 0; row <= static_cast<usize>(grid_rect.height()); ++row) {
        const auto start = top_left + shapes::Point{ 0, static_cast<int>(row * m_tile_size) };
        const auto end = shapes::Point{ bottom_right.x, start.y };
        service_provider.renderer().draw_line(start, end, grid_color);
        service_provider.renderer().draw_line(start - shapes::Point{ 0, 1 }, end - shapes::Point{ 0, 1 }, grid_color);
    }

    const auto outline_top_left = top_left - shapes::Point{ 2, 2 };
    const auto outline_bottom_right = shapes::Point{
        static_cast<int>(top_left.x + grid_rect.width() * m_tile_size + 1),
        static_cast<int>(top_left.y + grid_rect.height() * m_tile_size + 1),
    };

    const auto outline_rect = shapes::Rect{
        outline_top_left,
        outline_bottom_right,
    };
    service_provider.renderer().draw_rect_outline(outline_rect, border_color);
}
