


#include "command_line_arguments.hpp"


CommandLineArguments::CommandLineArguments(
        std::optional<std::filesystem::path> recording_path,
        std::optional<u32> target_fps,
        Level starting_level,
        bool silent
)
    : recording_path{ std::move(recording_path) },
      target_fps{ target_fps },
      starting_level{ starting_level },
      silent{ silent } { }
