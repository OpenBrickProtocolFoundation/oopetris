
#pragma once

#include <string>

#include "helper/parse_json.hpp"

namespace lobby {


    struct VersionResult {
        std::string version;
    };


    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VersionResult, version)


} // namespace lobby
