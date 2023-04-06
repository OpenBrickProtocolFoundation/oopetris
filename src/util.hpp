

#pragma once

#include <sstream>
#include <string>

namespace util {


    template<typename T>
    std::string to_hex_str(T number) {
        std::ostringstream ss{};
        ss << std::hex << number;
        return ss.str();
    }
} // namespace util
