#pragma once

#include <core/helper/expected.hpp>

#include <argparse/argparse.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <stdexcept>
#include <string>


struct Dump {
    bool ensure_ascii;
    bool pretty_print;
};

struct Info { };


struct CommandLineArguments final {
private:
public:
    std::filesystem::path recording_path;
    std::variant<Dump, Info> value;


    template<typename T>
    CommandLineArguments(std::filesystem::path&& recording_path, T&& value)
        : recording_path{ std::move(recording_path) },
          value{ std::forward<T>(value) } { }

    template<typename T>
    CommandLineArguments(std::filesystem::path&& recording_path, const T& value)
        : recording_path{ std::move(recording_path) },
          value{ value } { }


    [[nodiscard]] static helper::expected<CommandLineArguments, std::string> from_args(int argc, char** argv) noexcept {
        argparse::ArgumentParser parser{ argc >= 1 ? argv[0] //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                                                   : "oopetris_recording_utility",
                                         "0.0.1", argparse::default_arguments::all };


        parser.add_argument("-r", "--recording").help("the path of a recorded game file").required();


        // git add subparser
        argparse::ArgumentParser dump_parser("dump");
        dump_parser.add_description("Dump JSON value");
        dump_parser.add_argument("-a", "--ensure-ascii")
                .help("Only use ASCII characters and escape sequences (\\uXXXX)")
                .flag();
        dump_parser.add_argument("-p", "--pretty-print").help("Pretty print the JSON").flag();

        argparse::ArgumentParser info_parser("info");
        info_parser.add_description("Print hHuman readable Info");


        parser.add_subparser(dump_parser);
        parser.add_subparser(info_parser);

        try {

            parser.parse_args(argc, argv);

            auto recording_path = parser.get("--recording");

            if (parser.is_subcommand_used(dump_parser)) {
                const auto ensure_ascii = dump_parser.get<bool>("--ensure-ascii");
                const auto pretty_print = dump_parser.get<bool>("--pretty-print");


                return CommandLineArguments{
                    std::move(recording_path), Dump{ .ensure_ascii = ensure_ascii, .pretty_print = pretty_print }
                };
            }

            if (parser.is_subcommand_used(info_parser)) {
                return CommandLineArguments{
                    std::move(recording_path),
                    Info{},
                };
            }


            return helper::unexpected<std::string>{ "Unknown or no subcommand used" };

        } catch (const std::exception& error) {
            return helper::unexpected<std::string>{ error.what() };
        }
    }
};
