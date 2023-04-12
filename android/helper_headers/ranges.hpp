

#pragma once

#include "functional.hpp"
#include <algorithm>
#include <iterator>
namespace std {
    // this "implementation" is just some mapping to std:: calls from algorithm, it's only for convenience and not 100% tested and may be incorrect
    namespace ranges {

        using std::reverse;


        using std::end;


        //wrapper for std::find in algorithm, definitions from https://en.cppreference.com/w/cpp/algorithm/find and https://en.cppreference.com/w/cpp/algorithm/ranges/find
        //NOTE: some concepts in here are also C++20 (not usable in android), so I removed them, since std::find also checks for validity of inputs!
        //NOTE the 4. argument Proj isn't supported :(
        //NOTE: only 1 out of 3 possible usages is covered atm.
        /*  template<std::input_iterator I, std::sentinel_for<I> S, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate < ranges::equal_to, std::projected<I, Proj>, const T*> */
        template<class I, class S, class T, class Proj = std::identity>
        constexpr I find(I first, S last, const T& value, [[maybe_unused]] Proj proj = {}) {
            return std::find(first, last, value);
        }


        /*  template<ranges::input_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate < ranges::equal_to, std::projected<ranges::iterator_t<R>, Proj>, const T*> */
        template<class R, class T, class Proj = std::identity>
        /*    constexpr std::borrowed_iterator_t<R> */
        constexpr std::iterator<std::forward_iterator_tag, R>
        find(R&& r, const T& value, [[maybe_unused]] Proj proj = {}) {
            return std::find(std::begin(r), std::end(r), value);
        }


        //wrapper for std::all_of in algorithm, definitions from https://en.cppreference.com/w/cpp/algorithm/all_any_none_of and https://en.cppreference.com/w/cpp/algorithm/ranges/all_any_none_of
        //NOTE: some concepts in here are also C++20 (not usable in android), so I removed them, since std::all_of also checks for validity of inputs!
        //NOTE the 4. argument Proj isn't supported :(
        /*         template<
                std::input_iterator I,
                std::sentinel_for<I> S,
                class Proj = std::identity,
                std::indirect_unary_predicate<std::projected<I, Proj>> Pred> */
        template<class I, class S, class Proj = std::identity, class Pred>
        constexpr bool all_of(I first, S last, Pred pred, [[maybe_unused]] Proj proj = {}) {

            return std::all_of(first, last, pred);
        }


        /*         template<
                ranges::input_range R,
                class Proj = std::identity,
                std::indirect_unary_predicate<std::projected<ranges::iterator_t<R>, Proj>> Pred> */
        template<class R, class Proj = std::identity, class Pred>
        constexpr bool all_of(R&& r, Pred pred, [[maybe_unused]] Proj proj = {}) {

            return std::all_of(std::begin(r), std::end(r), pred);
        }


    } // namespace ranges
} // namespace std