#include "command_line_arguments.hpp"
#include "tetris_application.hpp"

int main(int argc, char** argv) {
    auto command_line_arguments = CommandLineArguments{ argc, argv };

    TetrisApplication tetris_app(std::move(command_line_arguments));
    tetris_app.run();

    return 0;
}
