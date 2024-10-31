
#include "./date.hpp"

#include <array>
#include <chrono>
#include <fmt/format.h>
#include <iomanip>

date::ISO8601Date::ISO8601Date(u64 value) : m_value{ value } { }

date::ISO8601Date::ISO8601Date(std::tm time_struct) {

    const std::time_t time = std::mktime(&time_struct);

    if (time < 0) {
        throw std::runtime_error("Couldn't convert std::tm to std::time_t");
    }

    m_value = static_cast<u64>(time);
}


date::ISO8601Date date::ISO8601Date::now() {
    auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    return ISO8601Date{ static_cast<u64>(time) };
}


helper::expected<date::ISO8601Date, std::string> date::ISO8601Date::from_string(const std::string& input) {


    std::tm time_struct = {};

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) \
        || defined(__SERENITY__)


    std::istringstream input_stream{ input };
    input_stream >> std::get_time(&time_struct, ISO8601Date::iso_8601_format_string);

    if (input_stream.fail()) {
        return helper::unexpected<std::string>{ "error calling std::get_time(): unable to convert input" };
    } else if (not input_stream.eof()) {
        return helper::unexpected<std::string>{ "error calling std::get_time(): didn't consume the whole input" };
    }

#else
    auto* const result = strptime(input.c_str(), ISO8601Date::iso_8601_format_string, &time_struct);

    if (result == nullptr) {
        return helper::unexpected<std::string>{ fmt::format("error calling strptime: {}", std::strerror(errno)) };
    }

    if (*result != '\0') {
        return helper::unexpected<std::string>{ "error getting date from string: didn't consume the whole input" };
    }

#endif

    // see why this is set: https://en.cppreference.com/w/cpp/chrono/c/mktime#Notes
    time_struct.tm_isdst = 0; // Not daylight saving

    return ISO8601Date{ time_struct };
}


[[nodiscard]] helper::expected<std::tm, std::string> date::ISO8601Date::get_tm_struct(std::time_t value) {

    std::tm time_struct{};
#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    if (gmtime_s(&time_struct, &value) != 0) {
        return helper::unexpected<std::string>{ "error calling gmtime_s" };
    }
#else
    if (gmtime_r(&value, &time_struct) == nullptr) {
        return helper::unexpected<std::string>{ fmt::format("error calling gmtime_r: {}", std::strerror(errno)) };
    }
#endif

    return time_struct;
}


[[nodiscard]] helper::expected<std::string, std::string>
date::ISO8601Date::format_tm_struct(std::tm time_struct, const char* format_string) {
    static constexpr auto buffer_size = usize{ 100 };
    std::array<char, buffer_size> buffer{};

    const auto result = std::strftime(buffer.data(), buffer.size(), format_string, &time_struct);
    if (result == 0) {
        return helper::unexpected<std::string>{ "error calling std::strftime" };
    }

    return std::string{ buffer.data() };
}

[[nodiscard]] helper::expected<std::string, std::string> date::ISO8601Date::to_string() const {

    const auto time_struct = get_tm_struct(static_cast<std::time_t>(m_value));
    if (not time_struct.has_value()) {
        return helper::unexpected<std::string>{ time_struct.error() };
    }

    const auto formatted = format_tm_struct(time_struct.value(), ISO8601Date::iso_8601_format_string);
    if (not formatted.has_value()) {
        return helper::unexpected<std::string>{ formatted.error() };
    }

    return formatted.value();
}

[[nodiscard]] u64 date::ISO8601Date::value() const {
    return m_value;
}
