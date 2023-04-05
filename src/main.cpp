#include "tetris_application.hpp"
#include <SDL.h>

int main(int, char**) {
    static constexpr int target_fps = 60;

    // TODO show buttons to select from one of the RunTypes

    TetrisApplication tetris_app = TetrisApplication{ RunType::OnlineMultiplayer, 2 };
    tetris_app.run(target_fps);

    return 0;
}
