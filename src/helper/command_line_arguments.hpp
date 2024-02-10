#pragma once

#include "constants.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <tl/optional.hpp>
#include <type_traits>

struct CommandLineArguments final {
private:
    static inline constexpr auto default_target_fps = u64{ 60 };
    static inline constexpr auto default_starting_level = i32{ 0 };

public:
    tl::optional<std::filesystem::path> recording_path{};
    std::remove_cvref_t<decltype(default_target_fps)> target_fps{ default_target_fps };
    std::remove_cvref_t<decltype(default_starting_level)> starting_level{ default_starting_level };
    bool silent{ false };

    CommandLineArguments(int argc, char** argv) {
        argparse::ArgumentParser parser{ constants::program_name, constants::version,
                                         argparse::default_arguments::all };
        parser.add_argument("-r", "--recording").help("the path of a recorded game used for replay");
        parser.add_argument("-f", "--target-fps")
                .help("the number of simulation steps per second")
                .scan<'i', decltype(target_fps)>()
                .default_value(default_target_fps);
        parser.add_argument("-l", "--level")
                .help("the starting level of the game")
                .scan<'i', decltype(starting_level)>()
                .default_value(default_starting_level);
        parser.add_argument("-s", "--silent").help("disable audio output").default_value(false).implicit_value(true);
        try {
            parser.parse_args(argc, argv);

            if (auto path = parser.present("--recording")) {
                spdlog::info("recording is present");
                recording_path = utils::get_root_folder() / *path;
            }

            const auto fps = parser.get<decltype(target_fps)>("--target-fps");
            if (fps >= 1) {
                target_fps = fps;
            } else {
                spdlog::error("invalid value for target fps ({}), using default value instead ({})", fps, target_fps);
            }

            const auto level = parser.get<decltype(starting_level)>("--level");
            if (level >= 0 and level <= 30) {
                starting_level = level;
            } else {
                spdlog::error(
                        "invalid value for starting level ({}), using default value instead ({})", level, starting_level
                );
            }

            silent = parser.get<bool>("--silent");
        } catch (const std::exception& err) {
            spdlog::error("error parsing command line arguments: {}", err.what());
#if defined(__ANDROID__)
            // calling exit() in android doesn't do the correct job, it completely avoids resource cleanup by the underlying SDLActivity.java
            // (java wrapper), that calls the main and expects it to return ALWAYS and throwing an exception in a catch statement is bad,
            // but is required here
            throw std::runtime_error{ "exit with status code 1: " + std::string{ err.what() } };
#else
            std::exit(1);
#endif
        }
    }
};
