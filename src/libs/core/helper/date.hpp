#pragma once

#include "./expected.hpp"
#include "./types.hpp"
#include "./windows.hpp"

#include <ctime>
#include <string>

namespace date {

    struct ISO8601Date {
    private:
        u64 m_value;

        ISO8601Date(std::tm tm);

        static constexpr const char* iso_8601_format_string = "%Y%m%dT%H%M%S";

    public:
        OOPETRIS_EXPORTED ISO8601Date(u64 value);

        OOPETRIS_EXPORTED static ISO8601Date now();
        OOPETRIS_EXPORTED static helper::expected<ISO8601Date, std::string> from_string(const std::string& input);

        OOPETRIS_EXPORTED [[nodiscard]] helper::expected<std::string, std::string> to_string() const;

        OOPETRIS_EXPORTED [[nodiscard]] u64 value() const;

    private:
        [[nodiscard]] static helper::expected<std::tm, std::string> get_tm_struct(std::time_t value);

        [[nodiscard]] static helper::expected<std::string, std::string>
        format_tm_struct(std::tm tm, const char* format_string);
    };


} // namespace date
