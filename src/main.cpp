#include "command_line_arguments.hpp"
#include "tetris_application.hpp"

int main(int argc, char** argv) {
    TetrisApplication tetris_app(CommandLineArguments{ argc, argv });
    tetris_app.run();

    return 0;
}