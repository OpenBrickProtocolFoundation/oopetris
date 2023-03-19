#include "SDL2/SDL.h"
#include "application.hpp"
#include "grid.hpp"
#include "mino.hpp"
#include <vector>

struct Tetris : public Application {
public:
    static constexpr int width = 800;
    static constexpr int height = 600;
    static constexpr int tile_size = 30;

    Tetris() : Application{ "Tetris", WindowPosition::Centered, width, height }, m_grid{ Point::zero(), tile_size } {
        // clang-format off
        m_grid.set(Point{ 0, 0 }, TetrominoType::I);
        m_grid.set(Point{ 0, 1 }, TetrominoType::I);
        m_grid.set(Point{ 0, 2 }, TetrominoType::I);
        m_grid.set(Point{ 3, 5 }, TetrominoType::I);
        m_grid.set(Point{ 3, 5 }, TetrominoType::J);
        m_grid.set(Point{ 3, 6 }, TetrominoType::Z);
        // clang-format on
    }

private:
    Grid m_grid;

protected:
    void update(double delta_time) override { }

    void render() const override {
        Application::render(); // call parent function to clear the screen
        m_grid.render(*this);
    }
};

int main(int, char**) {
    static constexpr int target_fps = 60;

    Tetris game;
    game.run(target_fps);

    return 0;
}
