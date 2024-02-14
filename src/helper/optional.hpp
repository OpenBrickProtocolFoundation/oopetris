
#pragma once

#ifdef __USE_TL_OPTIONAL
#include <tl/optional.hpp>
#else
#include <optional>
#endif


namespace helpers {

#ifdef __USE_TL_OPTIONAL

    template<typename T>
    using optional = tl::optional<T>;

    constexpr auto nullopt = tl::nullopt;

#else

    template<typename T>
    using optional = std::optional<T>;

    constexpr auto nullopt = std::nullopt;

#endif


} // namespace helpers
