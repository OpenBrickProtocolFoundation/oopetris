#pragma once

#include "application.hpp"
#include "mino.hpp"
#include "point.hpp"
#include <algorithm>
#include <vector>

struct Grid final {
public:
    static const int width;
    static const int height;
    static const int invisible_rows;
    static const int num_tiles;
    static const Point preview_background_position;
    static const Point preview_tetromino_position;
    static const Point preview_extends;
    static const Color background_color;
    static const Color border_color;

private:
    Point m_offset;
    int m_tile_size;
    std::vector<Mino> m_minos;

public:
    Grid(Point offset, int tile_size);
    Point tile_size() const;
    Point to_screen_coords(Point grid_coords) const;
    void render(const Application& app) const;
    void clear_row_and_let_sink(int row);
    bool is_empty(Point coordinates) const;
    void set(Point coordinates, TetrominoType type);

private:
    void draw_minos(const Application& app) const;
    void draw_preview_background(const Application& app) const;
    void draw_playing_field_background(const Application& app) const;
};
