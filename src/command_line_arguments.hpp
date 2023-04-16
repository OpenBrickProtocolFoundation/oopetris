#pragma once

#include "types.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <tl/optional.hpp>

struct PlayModeInformation {
    bool is_multiplayer;
    bool is_server;
    u8 player_nums;
};


struct CommandLineArguments {
private:
    static inline constexpr int default_target_fps = 60;
    static inline constexpr int default_starting_level = 0;

public:
    tl::optional<std::filesystem::path> recording_path{};
    u64 target_fps{ default_target_fps };
    i32 starting_level{ default_starting_level };
    PlayModeInformation play_mode_information;

    CommandLineArguments(int argc, char** argv) {
        argparse::ArgumentParser parser{ "oopetris", "0.0.1", argparse::default_arguments::all };
        parser.add_argument("-r", "--recording").help("the path of a recorded game used for replay");
        parser.add_argument("-f", "--target-fps")
                .help("the number of simulation steps per second")
                .scan<'i', int>()
                .default_value(default_target_fps);
        parser.add_argument("-l", "--level")
                .help("the starting level of the game")
                .scan<'i', int>()
                .default_value(default_starting_level);
        parser.add_argument("-m", "--multiplayer").help("play in multiplayer mode").scan<'i', int>();
        parser.add_argument("-s", "--server")
                .help("is this instance the server of the multiplayer")
                .default_value(false)
                .implicit_value(true);

        try {
            parser.parse_args(argc, argv);

            if (auto path = parser.present<std::string>("--recording")) {
                spdlog::info("recording is present");
                recording_path = std::filesystem::path{ *path };
            }

            const auto fps = parser.get<int>("--target-fps");
            if (fps >= 1) {
                target_fps = static_cast<u64>(fps);
            } else {
                spdlog::error("invalid value for target fps ({}), using default value instead ({})", fps, target_fps);
            }

            const auto level = parser.get<int>("--level");
            if (level >= 0 and level <= 30) {
                starting_level = level;
            } else {
                spdlog::error(
                        "invalid value for starting level ({}), using default value instead ({})", level, starting_level
                );
            }

            const auto multiplayer = parser.present<std::vector<int>>("--multiplayer");
            if (multiplayer.has_value()) {
                const auto multiplayer_num = parser.get<int>("--multiplayer");
                const auto is_server = parser.get<bool>("--server");
                play_mode_information = PlayModeInformation{ true, is_server, static_cast<u8>(multiplayer_num) };
            } else {
                play_mode_information = PlayModeInformation{ false, false, 0 };
            }


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
