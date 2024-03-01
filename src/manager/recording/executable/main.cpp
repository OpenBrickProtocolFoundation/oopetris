
#include "command_line_arguments.hpp"

#include <filesystem>
#include <iostream>


int main(int argc, char** argv) {

    const auto arguments = CommandLineArguments(argc, argv);

    //TODO build this utility
    std::cout << "got: " << arguments.recording_path << "\n";

    return 0;
}
