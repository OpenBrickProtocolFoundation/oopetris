#pragma once


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
    std::filesystem::path recording_path{};
    std::variant<Dump, Info> value;


    CommandLineArguments(int argc, char** argv) {
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

            this->recording_path = parser.get("--recording");

            if (parser.is_subcommand_used(dump_parser)) {
                const auto ensure_ascii = dump_parser.get<bool>("--ensure-ascii");
                const auto pretty_print = dump_parser.get<bool>("--pretty-print");

                this->value = Dump{ .ensure_ascii = ensure_ascii, .pretty_print = pretty_print };

            } else if (parser.is_subcommand_used(info_parser)) {
                this->value = Info{};
            } else {
                throw std::runtime_error("Unknown or no subcommand used");
            }
        } catch (const std::exception& error) {
            std::cerr << error.what();
            std::exit(1);
        }
    }
};
