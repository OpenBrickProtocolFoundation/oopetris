#include "tetris_application.hpp"
#include "command_line_arguments.hpp"

int main(int argc, char** argv) {
    const auto command_line_arguments = CommandLineArguments{ argc, argv };

    TetrisApplication tetris_app(command_line_arguments.recording_path);
    tetris_app.run(command_line_arguments.target_fps);

    return 0;
}
