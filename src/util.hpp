

#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace util {


    inline std::string to_hex_str(uint8_t number) {
        std::ostringstream ss{};

        ss << "0x" << std::hex << std::setfill('0');
        ss << std::hex << std::setw(2) << static_cast<int>(number);
        return ss.str();
    }

    inline std::string to_hex_str(uint32_t number) {
        std::ostringstream ss{};

        ss << "0x" << std::hex << std::setfill('0');
        ss << std::hex << std::setw(8) << static_cast<long>(number);
        return ss.str();
    }

} // namespace util
