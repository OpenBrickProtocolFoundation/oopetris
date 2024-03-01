
#include "command_line_arguments.hpp"
#include "recording_reader.hpp"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {

    const auto arguments = CommandLineArguments(argc, argv);

    if (not std::filesystem::exists(arguments.recording_path)) {
        std::cerr << arguments.recording_path << " does not exist!\n";
        return 1;
    }


    const auto parsed = recorder::RecordingReader::from_path(arguments.recording_path);

    return 0;
}
