#pragma once

#include "helper/expected.hpp"
#include "sol_include.hpp"

#include <filesystem>
#include <string>

namespace external {

    struct LUAHandler {
        //TODO
    };


    [[nodiscard]] helper::expected<LUAHandler, std::string> load_from_file(const std::filesystem::path& file);

} // namespace external
