
#pragma once

#include "helper/utils.hpp"

#include <cassert>
#include <type_traits>

// define a consteval assert, it isn't a pretty error message, but there's nothing we can do against that atm :(
// this https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2758r2.html tries to fix it
#define CONSTEVAL_ONLY_STATIC_ASSERT(CHECK, MSG) /*NOLINT(cppcoreguidelines-macro-usage)*/                                                      \
    ((CHECK) ? void(0) : [] {                                                                                                                   \
        /* If you see this really bad c++ error message, follow the origin of MSG, to see the real error message, c++ error messages suck xD */ \
        throw(MSG);                                                                                                                             \
    }())

#define CONSTEVAL_STATIC_ASSERT(CHECK, MSG)           \
    do { /*NOLINT(cppcoreguidelines-avoid-do-while)*/ \
        if (utils::is_constant_evaluated()) {         \
            CONSTEVAL_ONLY_STATIC_ASSERT(CHECK, MSG); \
        } else {                                      \
            assert(CHECK&& MSG);                      \
        }                                             \
    } while (false)


namespace const_utils {


#define PROPAGATE(val, V, E) /*NOLINT(cppcoreguidelines-macro-usage)*/       \
    do {                     /*NOLINT(cppcoreguidelines-avoid-do-while)*/    \
        if (not((val).has_value())) {                                        \
            return const_utils::expected<V, E>::error_result((val).error()); \
        }                                                                    \
    } while (false)

    // represents a sort of constexpr std::expected
    template<typename V, typename E>
        requires std::is_default_constructible_v<V> && std::is_default_constructible_v<E>
    struct expected {
    private:
        bool m_has_value;
        V m_value;
        E m_error;

        constexpr expected(
                bool has_value,
                const V& value,
                const E& error
        ) //NOLINT(modernize-pass-by-value)
            : m_has_value{ has_value },
              m_value{ value },
              m_error{ error } { }

    public:
        [[nodiscard]] constexpr static expected<V, E> good_result(const V& type) {
            return { true, type, E{} };
        }

        [[nodiscard]] constexpr static expected<V, E> error_result(const E& error) {
            return { false, V{}, error };
        }

        [[nodiscard]] constexpr bool has_value() const {
            return m_has_value;
        }

        [[nodiscard]] constexpr V value() const {
            CONSTEVAL_STATIC_ASSERT((has_value()), "value() call on expected without value");

            return m_value;
        }

        [[nodiscard]] constexpr E error() const {
            CONSTEVAL_STATIC_ASSERT((not has_value()), "error() call on expected without error");

            return m_error;
        }
    };

} // namespace const_utils