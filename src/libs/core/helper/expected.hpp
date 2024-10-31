
#pragma once

#ifdef _USE_TL_EXPECTED
#include <tl/expected.hpp>
#else
#include <expected>
#endif


namespace helper {

#ifdef _USE_TL_EXPECTED
    template<typename S, typename T>
    using expected = tl::expected<S, T>; //NOLINT(readability-identifier-naming)

    template<typename T>
    using unexpected = tl::unexpected<T>; //NOLINT(readability-identifier-naming)

#else
    template<typename S, typename T>
    using expected = std::expected<S, T>; //NOLINT(readability-identifier-naming)

    template<typename T>
    using unexpected = std::unexpected<T>; //NOLINT(readability-identifier-naming)

#endif


} // namespace helper
