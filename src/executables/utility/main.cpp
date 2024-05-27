
#include "./command_line_arguments.hpp"

#include <recordings/recordings.hpp>

#include <exception>
#include <filesystem>
#include <iostream>

void print_info(const recorder::RecordingReader& recording_reader) noexcept {
    //TODO(Totto): Implement, print basic information and final result for each simulation
    UNUSED(recording_reader);
    std::cerr << "NOT IMPLEMENTED\n";
}

void dump_json(const recorder::RecordingReader& recording_reader, bool pretty_print, bool ensure_ascii) noexcept {

    auto result = json::try_convert_to_json<recorder::RecordingReader>(recording_reader);

    if (not result.has_value()) {
        std::cerr << fmt::format("An error occurred during converting to json: {}\n", result.error());
        std::exit(1);
    }


    int indent = -1;
    char indent_char = ' ';

    if (pretty_print) {
        indent = 1;
        indent_char = '\t';
    }

    try {

        std::cout << result.value().dump(indent, indent_char, ensure_ascii);

    } catch (const std::exception& error) {
        std::cerr << error.what();
        std::exit(1);
    }

    if (pretty_print) {
        std::cout << "\n";
    }
}


int main(int argc, char** argv) noexcept {

    try {


        auto arguments_result = CommandLineArguments::from_args(argc, argv);

        if (not arguments_result.has_value()) {
            std::cerr << arguments_result.error();
            std::exit(1);
        }

        auto arguments = std::move(arguments_result.value());

        if (not std::filesystem::exists(arguments.recording_path)) {
            std::cerr << arguments.recording_path << " does not exist!\n";
            return 1;
        }


        auto parsed = recorder::RecordingReader::from_path(arguments.recording_path);

        if (not parsed.has_value()) {
            std::cerr << fmt::format(
                    "An error occurred during parsing of the recording file '{}': {}\n",
                    arguments.recording_path.string(), parsed.error()
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

    } catch (const std::exception& error) {
        std::cerr << error.what();
        return 1;
    }

    return 0;
}
