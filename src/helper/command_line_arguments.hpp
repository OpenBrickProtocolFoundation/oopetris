#pragma once

#include "helper/constants.hpp"
#include "helper/optional.hpp"
#include "helper/types.hpp"
#include "helper/utils.hpp"

#include <argparse/argparse.hpp>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <type_traits>

struct CommandLineArguments final {
private:
    static inline constexpr auto default_starting_level = u32{ 0 };

public:
    helper::optional<std::filesystem::path> recording_path{};
    helper::optional<u32> target_fps{};
    std::remove_cvref_t<decltype(default_starting_level)> starting_level{ default_starting_level };
    bool silent{ false };

    CommandLineArguments(const std::vector<std::string>& arguments) {
        argparse::ArgumentParser parser{ constants::program_name, constants::version,
                                         argparse::default_arguments::all };
        parser.add_argument("-r", "--recording").help("the path of a recorded game used for replay");
        parser.add_argument("-f", "--target-fps").help("the number of simulation steps per second").scan<'i', u32>();
        parser.add_argument("-l", "--level")
                .help("the starting level of the game")
                .scan<'i', decltype(starting_level)>()
                .default_value(default_starting_level);
        parser.add_argument("-s", "--silent").help("disable audio output").default_value(false).implicit_value(true);
        try {
            if (not arguments.empty()) {
                parser.parse_args(arguments);
            }


            if (auto path = parser.present("--recording")) {
                spdlog::info("recording is present");
                recording_path = utils::get_root_folder() / *path;
            }

            const auto fps = parser.present<u32>("--target-fps");
            if (fps.has_value()) {
                if (fps.value() >= 1) {
                    target_fps = fps.value();
                } else {
                    spdlog::error(
                            "invalid value for target fps ({}), using default value instead (VSYNC)", fps.value()
                    );
                }
            }

            const auto level = parser.get<decltype(starting_level)>("--level");
            if (level <= 30) {
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
