#include "command_line_arguments.hpp"
#include "local_multiplayer.hpp"
#include "play_mode.hpp"
#include "tetris_application.hpp"
#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <tl/optional.hpp>
#include <tuple>
#include <vector>


int main(int argc, char** argv) {
    TetrisApplication tetris_app(CommandLineArguments{ argc, argv });
    tetris_app.run();
}
