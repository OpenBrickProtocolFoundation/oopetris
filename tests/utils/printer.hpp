

#pragma once

#include "helper/expected.hpp"


#include <gtest/gtest.h>


namespace
#ifdef _USE_TL_EXPECTED
        tl
#else
        std
#endif
{

    // make helper::expected printable
    template<typename T, typename S>
    void PrintTo(const expected<T, S>& value, std::ostream* os) {
        if (value.has_value()) {
            *os << "<Expected.Value>: " << ::testing::PrintToString<T>(value.value());
        } else {
            *os << "<Expected.Error>: " << ::testing::PrintToString<S>(value.error());
        }
    }

    template<typename T, typename S>
    std::ostream& operator<<(std::ostream& os, const expected<T, S>& value) {
        PrintTo<T, S>(value);
        return os;
    }

} // namespace tl

namespace
#ifdef __USE_TL_OPTIONAL
        tl
#else
        std
#endif
{

    // make std::optional printable
    template<typename T>
    void PrintTo(const optional<T>& value, std::ostream* os) { //NOLINT(cert-dcl58-cpp)
        if (value.has_value()) {
            *os << "<Optional.Value>: " << ::testing::PrintToString<T>(value.value());
        } else {
            *os << "<Optional.Nullopt>";
        }
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const optional<T>& value) { //NOLINT(cert-dcl58-cpp)
        PrintTo<T>(value);
        return os;
    }

} // namespace tl
