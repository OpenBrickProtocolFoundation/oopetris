#pragma once

#include "helper/expected.hpp"
#include "helper/types.hpp"

#include <ctime>
#include <string>

namespace date {

    struct ISO8601Date {
    private:
        u64 m_value;
        
        ISO8601Date(std::tm tm);

        static constexpr const char* iso_8601_format_string = "%Y%m%dT%H%M%S";

    public:
        ISO8601Date(u64 value);

        static ISO8601Date now();
        static helper::expected<ISO8601Date, std::string> from_string(const std::string& input);

        [[nodiscard]] helper::expected<std::string, std::string> to_string() const;

        [[nodiscard]] u64 value() const;

    private:
        [[nodiscard]] helper::expected<std::tm, std::string> get_tm_struct(std::time_t value) const;

        [[nodiscard]] helper::expected<std::string, std::string> format_tm_struct(std::tm tm, const char* format_string)
                const;
    };


} // namespace date
