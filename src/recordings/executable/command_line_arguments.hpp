#pragma once


#include <argparse/argparse.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <stdexcept>
#include <string>

enum class Command { Info, Dump };

struct CommandLineArguments final {
private:
public:
    std::filesystem::path recording_path{};
    Command command{ Command::Info };

    CommandLineArguments(int argc, char** argv) {
        argparse::ArgumentParser parser{ argc >= 1 ? argv[0] //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                                                   : "oopetris_recording_utility",
                                         "0.0.1", argparse::default_arguments::all };

        parser.add_argument("command")
                .help("specify the command")
                .default_value(std::string{ "info" })
                .choices("info", "i", "dump", "d")
                .required();

        parser.add_argument("-r", "--recording").help("the path of a recorded game file").required();
        try {

            parser.parse_args(argc, argv);

            recording_path = parser.get("--recording");

            const auto command_string = parser.get<std::string>("command");


            if (command_string == "info" or command_string == "i") {
                command = Command::Info;
            } else if (command_string == "dump" || command_string == "d") {
                command = Command::Dump;
            } else {
                throw std::runtime_error(fmt::format("Unknown command: '{}'", command_string));
            }
        } catch (const std::exception& err) {
            std::cerr << err.what();
            std::exit(1);
        }
    }
};
