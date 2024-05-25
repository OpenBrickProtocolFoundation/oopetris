
#include "command_line_arguments.hpp"

#include <recording_json_wrapper.hpp>
#include <recording_reader.hpp>

#include <filesystem>
#include <iostream>

void print_info(const recorder::RecordingReader& recording_reader) noexcept {
    //TODO(Totto): Implement
    UNUSED(recording_reader);
    std::cerr << "NOT IMPLEMENTED\n";
}

void dump_json(const recorder::RecordingReader& recording_reader, bool pretty_print, bool ensure_ascii) noexcept {

    nlohmann::json json_value;

    //TODO: use parse_json helper

    nlohmann::adl_serializer<recorder::RecordingReader>::to_json(json_value, recording_reader);

    int indent = -1;
    char indent_char = ' ';

    if (pretty_print) {
        indent = 1;
        indent_char = '\t';
    }

    std::cout << json_value.dump(indent, indent_char, ensure_ascii);

    if (pretty_print) {
        std::cout << "\n";
    }
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

    std::visit(
            helper::overloaded{ [&recording_reader](const Dump& dump) {
                                   dump_json(recording_reader, dump.pretty_print, dump.ensure_ascii);
                               },
                                [&recording_reader](const Info& /* info */) { print_info(recording_reader); } },
            arguments.value
    );


    return 0;
}
