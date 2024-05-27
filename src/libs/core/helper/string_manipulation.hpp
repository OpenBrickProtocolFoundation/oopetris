
#pragma once

#include <string>
#include <vector>

namespace string {

    std::string to_lower_case(const std::string& input);

    std::string to_upper_case(const std::string& input);

    // for string delimiter
    std::vector<std::string> split_string_by_char(const std::string& start, const std::string& delimiter);

    // trim from start (in place)
    void ltrim(std::string& str);

    // trim from end (in place)
    void rtrim(std::string& str);

    void trim(std::string& str);

} // namespace string
