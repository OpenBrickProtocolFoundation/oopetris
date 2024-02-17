
#pragma once

#ifdef _USE_TL_EXPECTED
#include <tl/expected.hpp>
#else
#include <expected>
#endif


namespace helpers {

#ifdef _USE_TL_EXPECTED
    template<typename S, typename T>
    using expected = tl::expected<S, T>;

    template<typename T>
    using unexpected = tl::unexpected<T>;

#else
    template<typename S, typename T>
    using expected = std::expected<S, T>;

    template<typename T>
    using unexpected = std::unexpected<T>;

#endif


} // namespace helpers
