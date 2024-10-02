
#pragma once

#include "./windows.hpp"

#include <string>
#include <vector>

namespace string {

    OOPETRIS_EXPORTED std::string to_lower_case(const std::string& input);

    OOPETRIS_EXPORTED std::string to_upper_case(const std::string& input);

    // for string delimiter
    OOPETRIS_EXPORTED std::vector<std::string>
    split_string_by_char(const std::string& start, const std::string& delimiter);

    // trim from start (in place)
    OOPETRIS_EXPORTED void ltrim(std::string& str);

    // trim from end (in place)
    OOPETRIS_EXPORTED void rtrim(std::string& str);

    OOPETRIS_EXPORTED void trim(std::string& str);

} // namespace string
