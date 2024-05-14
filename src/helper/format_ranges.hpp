
#pragma once

#include "format.hpp"

#include <ranges>

#ifdef _USE_FMTLIB
#include <fmt/ranges.h>
#else
#include <format>
#include <sstream>
#endif


namespace helper {

    template<std::ranges::range V, typename CharT = char>
#ifdef _USE_FMTLIB
    inline auto join(const V& values, const CharT* separator) {
        return helper::join(values, separator)
    }
#else
#if __cplusplus >= 202302L
        requires std::formattable<V, CharT>
#endif
    auto join(const V& values, const char* separator) {
        //TODO: use this, if available __cpp_lib_format_ranges >= 202207L


        std::stringstream stream{};
        bool first = true;
        for (const auto& value : values) {
            stream << std::format("{}", value);
            if (!first) {
                stream << separator;
                continue;
            }

            first = false;
        }

        return stream.str();
    }

#endif

}; // namespace helper
