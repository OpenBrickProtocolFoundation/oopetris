#include "Utils.hpp"

#include <random>

//from: https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
std::string utils::randomAsciiString(std::string::size_type length) {

    //TODO: make this static in some way (or use ascii codepoints instead)
    constexpr auto& chrs =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{ std::random_device{}() };
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while (length--) {
        s += chrs[pick(rg)];
    }

    return s;
}
