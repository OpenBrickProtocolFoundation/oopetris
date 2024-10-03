
#include "parser.hpp"

#include <core/helper/expected.hpp>

#include "game/command_line_arguments.hpp"

#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"


#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>


helper::expected<CommandLineArguments, std::string> helper::parse_args(const std::vector<std::string>& arguments) {
    argparse::ArgumentParser parser{ constants::program_name, constants::version, argparse::default_arguments::all };
    parser.add_argument("-r", "--recording").help("the path of a recorded game used for replay");
    parser.add_argument("-f", "--target-fps").help("the number of simulation steps per second").scan<'i', u32>();
    parser.add_argument("-l", "--level")
            .help("the starting level of the game")
            .scan<'i', CommandLineArguments::Level>()
            .default_value(CommandLineArguments::default_starting_level);
    parser.add_argument("-s", "--silent").help("disable audio output").default_value(false).implicit_value(true);
    try {
        parser.parse_args(arguments);

        CommandLineArguments result{ std::nullopt, std::nullopt };


        if (auto path = parser.present("--recording")) {
            spdlog::info("recording is present");
            result.recording_path = utils::get_root_folder() / *path;
        }

        const auto fps = parser.present<u32>("--target-fps");
        if (fps.has_value()) {
            if (fps.value() >= 1) {
                result.target_fps = fps;
            } else {
                spdlog::error("invalid value for target fps ({}), using default value instead (VSYNC)", fps.value());
            }
        }
#if defined(__SERENITY__)
        // serenity OS can't handle vsync very well (Since it's inside qemu), so setting the target_fps value to 60 per default
        if (not result.target_fps.has_value()) {
            result.target_fps = 60;
        }
#endif


        const auto level = parser.get<CommandLineArguments::Level>("--level");
        if (level <= 30) {
            result.starting_level = level;
        } else {
            spdlog::error(
                    "invalid value for starting level ({}), using default value instead ({})", level,
                    CommandLineArguments::default_starting_level
            );
            result.starting_level = CommandLineArguments::default_starting_level;
        }

        result.silent = parser.get<bool>("--silent");

        return result;
    } catch (const std::exception& error) {
        return helper::unexpected<std::string>{ error.what() };
    }
}
