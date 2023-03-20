#include "tetris_application.hpp"
#include <SDL.h>

int main(int, char**) {
    static constexpr int target_fps = 60;

    TetrisApplication tetris_app;
    tetris_app.run(target_fps);

    return 0;
}
