


#include "command_line_arguments.hpp"


CommandLineArguments::CommandLineArguments(
        std::optional<std::filesystem::path> recording_path_a,
        std::optional<u32> target_fps_a,
        Level starting_level_a,
        bool silent_a
)
    : recording_path{ std::move(recording_path_a) },
      target_fps{ target_fps_a },
      starting_level{ starting_level_a },
      silent{ silent_a } { }
