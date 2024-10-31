#pragma once

#include <cassert>
#include <ostream>
#include <type_traits>

#include "../helper/types.hpp"

namespace shapes {
    template<typename T>
    struct AbstractPoint final {
        using Type = T;

        T x;
        T y;

        constexpr AbstractPoint() : AbstractPoint{ 0, 0 } { }
        constexpr AbstractPoint(T x_pos, T y_pos)
            : x{ x_pos },
              y{ y_pos } { } // NOLINT(bugprone-easily-swappable-parameters)

        static constexpr AbstractPoint<T> zero() {
            return AbstractPoint<T>{ 0, 0 };
        }

        constexpr bool operator==(AbstractPoint<T> rhs) const {
            return x == rhs.x and y == rhs.y;
        }

        constexpr bool operator!=(AbstractPoint<T> rhs) const {
            return not(*this == rhs);
        }

        constexpr AbstractPoint<T> operator*(T scale) const {
            return AbstractPoint<T>{ x * scale, y * scale };
        }

        constexpr AbstractPoint<T> operator/(T divisor) const {
            return AbstractPoint<T>{ x / divisor, y / divisor };
        }

        constexpr AbstractPoint<T> operator+(AbstractPoint<T> rhs) const {
            return AbstractPoint<T>{ static_cast<T>(x + rhs.x), static_cast<T>(y + rhs.y) };
        }

        constexpr AbstractPoint<T> operator+() const {
            return *this;
        }

        constexpr AbstractPoint<T> operator-(AbstractPoint<T> rhs) const {
            if constexpr (std::is_signed_v<T>) {
                return *this + (-rhs);
            } else {
                assert(x >= rhs.x && y >= rhs.y && "underflow in subtraction");
                return AbstractPoint<T>{ static_cast<T>(x - rhs.x), static_cast<T>(y - rhs.y) };
            }
        }

        constexpr AbstractPoint<T> operator+=(AbstractPoint<T> rhs) {
            *this = *this + rhs;
            return *this;
        }

        constexpr AbstractPoint<T> operator-=(AbstractPoint<T> rhs) {
            *this = *this - rhs;
            return *this;
        }

        template<typename S>
        constexpr AbstractPoint<S> cast() const {

#if !defined(NDEBUG)
            if constexpr (std::is_signed_v<T> != std::is_signed_v<S>) {
                if constexpr (std::is_signed_v<T> and not std::is_signed_v<S>) {
                    // source is signed, destination is unsigned, so both checks are necessary

                    assert(x >= static_cast<T>(0) && y >= static_cast<T>(0) && "cast invalid, value to small");
                    assert(static_cast<S>(x) <= std::numeric_limits<S>::max()
                           && static_cast<S>(y) <= std::numeric_limits<S>::max() && "cast invalid, value to big");

                } else {
                    // source is unsigned, destination is signed, so only the max check is necessary

                    assert(x <= std::numeric_limits<S>::max() && y <= std::numeric_limits<S>::max()
                           && "cast invalid, value to big");
                }
            } else {
                if constexpr (std::is_signed_v<T> and std::is_signed_v<S>) {
                    // both are signed, so both checks are necessary

                    assert(x >= std::numeric_limits<S>::min() && y >= std::numeric_limits<S>::min()
                           && "cast invalid, value to small");
                    assert(static_cast<S>(x) <= std::numeric_limits<S>::max()
                           && static_cast<S>(y) <= std::numeric_limits<S>::max() && "cast invalid, value to big");
                } else {
                    // both are unsigned, so no min check is necessary
                    assert(x <= std::numeric_limits<S>::max() && y <= std::numeric_limits<S>::max()
                           && "cast invalid, value to big");
                }
            }
#endif

            return AbstractPoint<S>{ static_cast<S>(x), static_cast<S>(y) };
        }

        template<typename S>
        constexpr AbstractPoint<S> cast_truncate() const {

            auto x_final = x;

            if (x < std::numeric_limits<S>::min()) {
                x_final = std::numeric_limits<S>::min();
            } else if (x > std::numeric_limits<S>::max()) {
                x_final = std::numeric_limits<S>::max();
            }

            auto y_final = y;

            if (y < std::numeric_limits<S>::min()) {
                y_final = std::numeric_limits<S>::min();
            } else if (y > std::numeric_limits<S>::max()) {
                y_final = std::numeric_limits<S>::max();
            }

            return AbstractPoint<S>{ static_cast<S>(x_final), static_cast<S>(y_final) };
        }
    };

    template<typename T>
        requires std::is_signed_v<T>
    constexpr AbstractPoint<T> operator-(AbstractPoint<T> point) {
        return AbstractPoint<T>{ static_cast<T>(-point.x), static_cast<T>(-point.y) };
    }


    template<typename T>
    constexpr AbstractPoint<T> operator*(T scale, AbstractPoint<T> point) {
        return point * scale;
    }

    template<typename S, typename T, typename R = S>
        requires(std::is_signed_v<S> == std::is_signed_v<T> && std::is_signed_v<T> == std::is_signed_v<R>)
    constexpr AbstractPoint<R> operator+(AbstractPoint<S> lhs, AbstractPoint<T> rhs) {
        return AbstractPoint<R>{ static_cast<R>(static_cast<R>(lhs.x) + static_cast<R>(rhs.x)),
                                 static_cast<R>(static_cast<R>(lhs.y) + static_cast<R>(rhs.y)) };
    }

    template<typename T>
    inline std::ostream& operator<<(std::ostream& ostream, const AbstractPoint<T>& point) {
        using PrintType = typename std::conditional_t<std::is_same_v<T, u8>, u32, T>;
        ostream << static_cast<PrintType>(point.x) << ", " << static_cast<PrintType>(point.y);
        return ostream;
    }

    using IPoint = AbstractPoint<i32>;
    using UPoint = AbstractPoint<u32>;

} // namespace shapes
