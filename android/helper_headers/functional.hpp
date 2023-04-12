

#pragma once

#include <utility>

namespace std {

    //from: https://github.com/llvm/llvm-project/blob/main/libcxx/include/__functional/identity.h#L40-L48
    struct identity {
        template<class _Tp>
        _LIBCPP_NODISCARD_EXT constexpr _Tp&& operator()(_Tp&& __t) const noexcept {
            return _VSTD::forward<_Tp>(__t);
        }

        using is_transparent = void;
    };
} // namespace std