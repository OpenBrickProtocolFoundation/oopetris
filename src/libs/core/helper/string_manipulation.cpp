
#include "./string_manipulation.hpp"

#include <algorithm>
#include <ranges>

std::string string::to_lower_case(const std::string& input) {
    auto result = input;
    for (auto& elem : result) {
        elem = static_cast<char>(std::tolower(elem));
    }

    return result;
}

std::string string::to_upper_case(const std::string& input) {
    auto result = input;
    for (auto& elem : result) {
        elem = static_cast<char>(std::toupper(elem));
    }

    return result;
}


// for string delimiter
std::vector<std::string> string::split_string_by_char(const std::string& start, const std::string& delimiter) {
    size_t pos_start = 0;
    size_t pos_end = 0;
    const auto delim_len = delimiter.length();

    std::vector<std::string> res{};

    while ((pos_end = start.find(delimiter, pos_start)) != std::string::npos) {
        auto token = start.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(start.substr(pos_start));
    return res;
}


void string::ltrim(std::string& str) {
    str.erase(str.begin(), std::ranges::find_if(str, [](unsigned char chr) { return std::isspace(chr) == 0; }));
}


void string::rtrim(std::string& str) {
    str.erase(
            std::ranges::find_if(
                    std::ranges::reverse_view(str), [](unsigned char chr) { return std::isspace(chr) == 0; }
            ).base(),
            str.end()
    );
}

void string::trim(std::string& str) {
    ltrim(str);
    rtrim(str);
}
