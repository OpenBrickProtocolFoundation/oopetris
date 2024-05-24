
#include "command_line_arguments.hpp"
#include "recording_json_wrapper.hpp"
#include "recording_reader.hpp"

#include <filesystem>
#include <iostream>

void print_info(const recorder::RecordingReader& recording_reader) {
    //TODO(Totto): Implement
    UNUSED(recording_reader);
    std::cerr << "NOT IMPLEMENTED\n";
}

void dump_json(const recorder::RecordingReader& recording_reader) {

    nlohmann::json json_value;

    nlohmann::adl_serializer<recorder::RecordingReader>::to_json(json_value, recording_reader);

    json_value.dump();
}


int main(int argc, char** argv) noexcept {

    const auto arguments = CommandLineArguments(argc, argv);

    if (not std::filesystem::exists(arguments.recording_path)) {
        std::cerr << arguments.recording_path << " does not exist!\n";
        return 1;
    }


    auto parsed = recorder::RecordingReader::from_path(arguments.recording_path);

    if (not parsed.has_value()) {
        std::cerr << fmt::format(
                "An error occurred during parsing of the recording file '{}': {}\n", arguments.recording_path.string(),
                parsed.error()
        );
        return 1;
    }


    const auto recording_reader = std::move(parsed.value());

    switch (arguments.command) {
        case Command::Info:
            print_info(recording_reader);
            return 0;
        case Command::Dump:
            dump_json(recording_reader);
            return 0;
        default:
            std::cerr << "Unknown command: ?\n";

            return 1;
    }


    return 0;
}
