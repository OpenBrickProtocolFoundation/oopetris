#pragma once

#include <cassert>
#include <ostream>
#include <type_traits>

#include "helper/types.hpp"

namespace shapes {
    template<typename T>
    struct AbstractPoint final {
        T x;
        T y;

        constexpr AbstractPoint() : AbstractPoint{ 0, 0 } { }
        constexpr AbstractPoint(T x, T y) : x{ x }, y{ y } { } // NOLINT(bugprone-easily-swappable-parameters)

        static constexpr AbstractPoint<T> zero() {
            return AbstractPoint<T>{ 0, 0 };
        }

        inline constexpr bool operator==(AbstractPoint<T> rhs) const {
            return x == rhs.x and y == rhs.y;
        }

        inline constexpr bool operator!=(AbstractPoint<T> rhs) const {
            return not(*this == rhs);
        }

        inline constexpr AbstractPoint<T> operator*(T scale) const {
            return AbstractPoint<T>{ x * scale, y * scale };
        }

        inline constexpr AbstractPoint<T> operator/(T divisor) const {
            return AbstractPoint<T>{ x / divisor, y / divisor };
        }

        inline constexpr AbstractPoint<T> operator+(AbstractPoint<T> rhs) const {
            return AbstractPoint<T>{ x + rhs.x, y + rhs.y };
        }

        inline constexpr AbstractPoint<T> operator+() const {
            return *this;
        }

        inline constexpr AbstractPoint<T> operator-(AbstractPoint<T> rhs) const {
            if constexpr (std::is_signed<T>::value) {
                return *this + (-rhs);
            }

            assert(x >= rhs.x && y >= rhs.y && "underflow in subtraction");
            return AbstractPoint<T>{ x - rhs.x, y - rhs.y };
        }

        inline constexpr AbstractPoint<T> operator+=(AbstractPoint<T> rhs) {
            *this = *this + rhs;
            return *this;
        }

        inline constexpr AbstractPoint<T> operator-=(AbstractPoint<T> rhs) {
            *this = *this - rhs;
            return *this;
        }

        template<typename S>
        inline constexpr AbstractPoint<S> cast() const {
            if constexpr (std::is_signed<T>::value and not std::is_signed<T>::value) {
                assert(x >= 0 && y >= 0 && "Not allowed to cast away negative number into an unsigned type");
            }
            return AbstractPoint<S>{ static_cast<S>(x), static_cast<S>(y) };
        }
    };

    template<typename T>
        requires std::is_signed_v<T>
    inline constexpr AbstractPoint<T> operator-(AbstractPoint<T> point) {
        return AbstractPoint<T>{ static_cast<T>(-point.x), static_cast<T>(-point.y) };
    }


    template<typename T>
    inline constexpr AbstractPoint<T> operator*(T scale, AbstractPoint<T> point) {
        return point * scale;
    }

    template<typename S, typename T, typename R = S>
        requires(std::is_signed_v<S> == std::is_signed_v<T> && std::is_signed_v<T> == std::is_signed_v<R>)
    inline constexpr AbstractPoint<R> operator+(AbstractPoint<S> lhs, AbstractPoint<T> rhs) {
        return AbstractPoint<R>{ static_cast<R>(lhs.x) + static_cast<R>(rhs.x),
                                 static_cast<R>(lhs.y) + static_cast<R>(rhs.y) };
    }

    template<typename T>
    inline std::ostream& operator<<(std::ostream& ostream, const AbstractPoint<T>& point) {
        ostream << point.x << ", " << point.y;
        return ostream;
    }

    using IPoint = AbstractPoint<i32>;
    using UPoint = AbstractPoint<u32>;

} // namespace shapes
