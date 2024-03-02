#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace external {

    void load_file(const std::filesystem::path& file);

}
