

#pragma once

#include <functional>
#include <type_traits>


namespace std {

    // taken from llvm: https://github.com/llvm/llvm-project/blob/main/libcxx/include/__concepts/arithmetic.h#L27-L30
    // [concepts.arithmetic], arithmetic concepts

    template<class _Tp>
    concept integral = std::is_integral_v<_Tp>;

    //from: https://github.com/llvm/llvm-project/blob/main/libcxx/include/__concepts/invocable.h#L24-L29
    // [concept.invocable]

    template<class _Fn, class... _Args>
    concept invocable = requires(_Fn&& __fn, _Args&&... __args) {
                            _VSTD::invoke(
                                    _VSTD::forward<_Fn>(__fn), _VSTD::forward<_Args>(__args)...
                            ); // not required to be equality preserving
                        };


} // namespace std
