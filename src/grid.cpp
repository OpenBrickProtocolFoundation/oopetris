#include "grid.hpp"

constexpr int Grid::width = 10;
constexpr int Grid::height = 22;
constexpr int Grid::invisible_rows = 2;
constexpr int Grid::num_tiles = width * height;
constexpr Point Grid::preview_background_position{ width + 1, 1 + invisible_rows };
constexpr Point Grid::preview_tetromino_position = preview_background_position + Point{ 0, 1 };
constexpr Point Grid::preview_extends{ 4, 4 };
constexpr Color Grid::background_color{ 12, 12, 12 };
constexpr Color Grid::border_color{ 42, 42, 42 };

Grid::Grid(Point offset, int tile_size) : m_offset{ offset - Point{ 0, invisible_rows * tile_size }}, m_tile_size{ tile_size } {
    m_minos.reserve(num_tiles);
}

Point Grid::tile_size() const {
    return Point{ m_tile_size, m_tile_size };
}

Point Grid::to_screen_coords(Point grid_coords) const {
    return m_offset + grid_coords * m_tile_size;
}

void Grid::render(const Application& app) const {
    draw_playing_field_background(app);
    draw_preview_background(app);
    draw_minos(app);
}

void Grid::draw_minos(const Application& app) const {
    for (const Mino& mino : m_minos) {
        if (mino.position().y >= invisible_rows) {
            mino.render(app, *this);
        }
    }
}

void Grid::draw_preview_background(const Application& app) const {
    const Point preview_top_left = to_screen_coords(preview_background_position);
    const Point preview_bottom_right =
            preview_top_left + Point{ tile_size().x * preview_extends.x - 1, tile_size().y * preview_extends.y - 1 };
    app.renderer().draw_rect_filled(Rect{ preview_top_left, preview_bottom_right }, background_color);

    const Point outline_top_left = preview_top_left - Point{ 1, 1 };
    const Point outline_bottom_right = preview_bottom_right + Point{ 1, 1 };
    const Rect outline_rect = Rect{ outline_top_left, outline_bottom_right };
    app.renderer().draw_rect_outline(outline_rect, border_color);
}

void Grid::draw_playing_field_background(const Application& app) const {
    const Point bottom_right{
        width * m_tile_size - 1,
        (height - invisible_rows) * m_tile_size,
    };
    app.renderer().draw_rect_filled(Rect{ Point::zero(), bottom_right }, background_color);
    app.renderer().draw_line(
            Point{ bottom_right.x + 1, 0 }, Point{ bottom_right.x + 1, app.window().size().y - 1 }, border_color
    );
}

void Grid::set(Point coordinates, TetrominoType type) {
    const Mino to_insert = Mino{ coordinates, type };
    for (Mino& current : m_minos) {
        if (current.position() == coordinates) {
            current = to_insert;
            return;
        }
    }
    m_minos.push_back(to_insert);
}

void Grid::clear_row_and_let_sink(int row) {
    m_minos.erase(
            std::remove_if(m_minos.begin(), m_minos.end(), [&](const Mino& mino) { return mino.position().y == row; }),
            m_minos.end()
    );
    for (Mino& mino : m_minos) {
        if (mino.position().y < row) {
            ++mino.position().y;
        }
    }
}

bool Grid::is_empty(Point coordinates) const {
    for (const Mino& mino : m_minos) {
        if (mino.position() == coordinates) {
            return false;
        }
    }
    return true;
}
