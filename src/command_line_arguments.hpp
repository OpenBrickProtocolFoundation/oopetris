#pragma once

#include "spdlog/spdlog.h"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <string>
#include <tl/optional.hpp>

struct CommandLineArguments {
private:
    static inline constexpr int default_target_fps = 60;

public:
    tl::optional<std::filesystem::path> recording_path{};
    u64 target_fps;

    CommandLineArguments(int argc, char** argv) {
        argparse::ArgumentParser parser{ "oopetris", "0.0.1", argparse::default_arguments::all };
        parser.add_argument("-r", "--recording").help("the path of a recorded game used for replay");
        parser.add_argument("-f", "--target-fps")
                .help("the number of simulation steps per second")
                .scan<'i', int>()
                .default_value(default_target_fps);
        try {
            parser.parse_args(argc, argv);

            if (auto path = parser.present("--recording")) {
                spdlog::info("recording is present");
                recording_path = std::filesystem::path{ *path };
            }

            const auto value = parser.get<int>("--target-fps");
            if (value >= 1) {
                target_fps = static_cast<u64>(value);
            } else {
                spdlog::error("invalid value for target fps ({}), using default value instead ({})", value, target_fps);
            }
        } catch (const std::exception& err) {
            spdlog::error("error parsing command line arguments: {}", err.what());
            std::exit(1);
        }
    }
};
