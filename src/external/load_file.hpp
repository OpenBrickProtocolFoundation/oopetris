#pragma once

#include "helper/expected.hpp"
#include "sol_include.hpp"

#include <filesystem>
#include <string>

namespace external {

    helper::expected<bool, std::string> load_file(const std::filesystem::path& file);

} // namespace external
