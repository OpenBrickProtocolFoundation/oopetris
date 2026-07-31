

#pragma once

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>
#include <core/helper/utils.hpp>

#include "../helper/export_symbols.hpp"

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

    OOPETRIS_GRAPHICS_EXPORTED CommandLineArguments(
            std::optional<std::filesystem::path> recording_path_a,
            std::optional<u32> target_fps_a,
            Level starting_level_a = default_starting_level,
            bool silent_a = default_silent
    );
};
