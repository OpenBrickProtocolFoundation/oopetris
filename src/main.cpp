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
        minos.emplace_back(Point::zero(), TetrominoType::I);
        minos.emplace_back(Point{ 1, 1 }, TetrominoType::J);
        minos.emplace_back(Point{ 1, 3 }, TetrominoType::L);
        minos.emplace_back(Point{ 3, 1 }, TetrominoType::O);
        minos.emplace_back(Point{ 3, 1 }, TetrominoType::S);
        minos.emplace_back(Point{ 2, 7 }, TetrominoType::T);
        minos.emplace_back(Point{ 4, 6 }, TetrominoType::Z);
    }

private:
    Grid m_grid;
    std::vector<Mino> minos;

protected:
    void update(double delta_time) override {
    }

    void render() const override {
        Application::render();
        for (const auto& mino : minos) {
            mino.render(*this, m_grid);
        }
    }
};

int main(int, char**) {
    static constexpr int target_fps = 60;

    Tetris game;
    game.run(target_fps);

    return 0;
}
