#pragma once


#include "./types.hpp"

#include <bit>
#include <cassert>
#include <climits>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace helper {

    template<class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
} // namespace helper


namespace utils {
    // taken from llvm: https://github.com/llvm/llvm-project/blob/main/libcxx/include/__concepts/arithmetic.h#L27-L30
    // [concepts.arithmetic], arithmetic concepts
    template<class T>
    concept integral = std::is_integral_v<T>;

    template<integral Integral>
    [[nodiscard]] constexpr Integral byte_swap(Integral value) noexcept {
        // based on source: slartibartswift
        auto result = Integral{};
        for (usize i = 0; i < sizeof(Integral); ++i) {
            result <<= CHAR_BIT;
            result |= value & static_cast<Integral>(std::numeric_limits<unsigned char>::max());
            value >>= CHAR_BIT;
        }
        return result;
    }

    [[nodiscard]] constexpr auto to_little_endian(integral auto value) {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return byte_swap(value);
        }
    }

    [[nodiscard]] constexpr auto from_little_endian(integral auto value) {
        return to_little_endian(value);
    }

    template<class Enum>
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum enum_) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(enum_);
    }

#if __cpp_lib_unreachable >= 202202L
    [[noreturn]] inline void unreachable() {
        std::unreachable();
    }
#else
    [[noreturn]] inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
        __assume(false);
#else                                        // GCC, Clang
        __builtin_unreachable();
#endif
    }
#endif
    template<size_t T>
    struct size_t_identity {
        //using type = T;
    };


    [[nodiscard]] constexpr bool is_constant_evaluated() noexcept {

#if defined(__cpp_if_consteval) && __cpp_if_consteval >= 201811L

        if consteval {
            return true;
        }

        return false;
#else
        return std::is_constant_evaluated();
#endif
    }


    template<typename T, typename S>
    constexpr std::optional<T*> is_child_class(S* parent) {
        auto* result = dynamic_cast<T*>(parent);

        if (result == nullptr) {
            return std::nullopt;
        }

        return result;
    }

    template<typename T, typename S>
    constexpr std::optional<T*> is_child_class(const std::unique_ptr<S>& parent) {
        return is_child_class<T, S>(parent.get());
    }

    template<typename T, typename S>
    constexpr std::optional<T*> is_child_class(const std::shared_ptr<S>& parent) {
        return is_child_class<T, S>(parent.get());
    }

    template<class T, class = void>
    struct IsIterator : std::false_type { };

    template<class T>
    struct IsIterator<T, std::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type { };

#if __cpp_static_assert >= 202306L
#define STATIC_ASSERT_WITH_MESSAGE(check, msg) static_assert(check, msg); // NOLINT(cppcoreguidelines-macro-usage)
#else
#define STATIC_ASSERT_WITH_MESSAGE(check, msg) static_assert(check); // NOLINT(cppcoreguidelines-macro-usage)
#endif


} // namespace utils

#define UNUSED(x) (void(x)) // NOLINT(cppcoreguidelines-macro-usage)

#if !defined(NDEBUG)
#define UNREACHABLE()                             /* NOLINT(cppcoreguidelines-macro-usage)*/                       \
    do {                                          /* NOLINT(cppcoreguidelines-avoid-do-while)*/                    \
        std::cerr << "UNREACHABLE " << (__FILE__) /* NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)*/ \
                  << ":" << (__LINE__) << " - "                                                                    \
                  << (__FUNCTION__) /* NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)*/               \
                  << "\n";                                                                                         \
        utils::unreachable();                                                                                      \
    } while (false)
#else
#define UNREACHABLE() utils::unreachable() // NOLINT(cppcoreguidelines-macro-usage)
#endif
// NOLINT(cppcoreguidelines-macro-usage)


#if defined(NDEBUG)
#define ASSERT(x) (UNUSED(x)) // NOLINT(cppcoreguidelines-macro-usage)
#else
#define ASSERT(x) assert(x) // NOLINT(cppcoreguidelines-macro-usage)
#endif
