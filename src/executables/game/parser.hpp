#pragma once

#include <core/helper/expected.hpp>

#include "game/command_line_arguments.hpp"


#include <string>
#include <vector>


namespace helper {
    helper::expected<CommandLineArguments, std::string> parse_args(const std::vector<std::string>& arguments);
} // namespace helper
