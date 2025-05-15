
#pragma once

#include "./export_symbols.hpp"

#include <string>
#include <vector>

namespace string {

    OOPETRIS_CORE_EXPORTED std::string to_lower_case(const std::string& input);

    OOPETRIS_CORE_EXPORTED std::string to_upper_case(const std::string& input);

    // for string delimiter
    OOPETRIS_CORE_EXPORTED std::vector<std::string>
    split_string_by_char(const std::string& start, const std::string& delimiter);

    // trim from start (in place)
    OOPETRIS_CORE_EXPORTED void ltrim(std::string& str);

    // trim from end (in place)
    OOPETRIS_CORE_EXPORTED void rtrim(std::string& str);

    OOPETRIS_CORE_EXPORTED void trim(std::string& str);

} // namespace string
