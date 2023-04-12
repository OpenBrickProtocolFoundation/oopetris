

#pragma once

#include <utility>
#include <type_traits>

namespace compatibility {

#ifdef __cpp_lib_to_underlying

    using std::to_underlying;

#else

    // from: https://en.cppreference.com/w/cpp/utility/to_underlying
    template<class Enum>
    constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum> >(e);
    }


#endif


} // namespace compatibility