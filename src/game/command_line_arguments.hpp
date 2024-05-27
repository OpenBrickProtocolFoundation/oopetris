

#pragma once

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>
#include <core/helper/utils.hpp>

#include <filesystem>
#include <optional>

struct CommandLineArguments final {

    static const constexpr auto default_starting_level = u32{ 0 };
    static const constexpr auto default_silent = true;

    std::optional<std::filesystem::path> recording_path;
    std::optional<u32> target_fps;
    using Level = std::remove_cvref_t<decltype(default_starting_level)>;
    Level starting_level;
    bool silent;

    CommandLineArguments(
            std::optional<std::filesystem::path> recording_path,
            std::optional<u32> target_fps,
            Level starting_level = default_starting_level,
            bool silent = default_silent
    );
};
