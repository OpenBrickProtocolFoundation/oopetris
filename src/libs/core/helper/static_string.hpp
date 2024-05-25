#pragma once

#include "./helper/types.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <string>
#include <string_view>

template<usize data_size>
struct StaticString {
private:
    std::array<char, data_size> m_data;

    constexpr StaticString() : m_data{} { }

public:
    constexpr StaticString(const char (&chars // NOLINT(modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
    )[data_size]) {
        std::copy(chars, chars + data_size, begin());
    }

    [[nodiscard]] constexpr usize size() const {
        return data_size - 1;
    }

    [[nodiscard]] constexpr usize length() const {
        return size();
    }

    [[nodiscard]] constexpr char* begin() {
        return m_data.data();
    }

    [[nodiscard]] constexpr char* end() {
        return begin() + length();
    }

    [[nodiscard]] constexpr const char* begin() const {
        return m_data.data();
    }

    [[nodiscard]] constexpr const char* end() const {
        return m_data.data() + size();
    }

    [[nodiscard]] constexpr const char* cbegin() const {
        return begin();
    }

    [[nodiscard]] constexpr const char* cend() const {
        return end();
    }

    [[nodiscard]] constexpr char& front() {
        return m_data.front();
    }

    [[nodiscard]] constexpr const char& front() const {
        return m_data.front();
    }

    [[nodiscard]] constexpr char& back() {
        return m_data.back();
    }

    [[nodiscard]] constexpr const char& back() const {
        return m_data.back();
    }

    template<usize other_data_size, typename Result = StaticString<data_size + other_data_size - 1>>
    [[nodiscard]] constexpr Result operator+(const StaticString<other_data_size>& other) const {
        auto concatenated = Result{};
        std::ranges::copy(*this, concatenated.begin());
        std::ranges::copy(other, concatenated.begin() + size());
        concatenated.back() = '\0';
        return concatenated;
    }

    [[nodiscard]] std::string string() const {
        return std::string{ cbegin(), cend() };
    }

    [[nodiscard]] constexpr const char* c_str() const {
        return cbegin();
    }

    [[nodiscard]] constexpr std::string_view string_view() const {
        return std::string_view{ cbegin(), cend() };
    }

    template<usize other_data_size>
    [[nodiscard]] friend constexpr bool
    operator==(const StaticString<data_size>& self, const StaticString<other_data_size>& other) {
        return self.m_data == other.m_data;
    }

    template<usize other_data_size>
    [[nodiscard]] friend constexpr bool
    operator!=(const StaticString<data_size>& self, const StaticString<other_data_size>& other) {
        return not(self == other);
    }

    template<usize first_data_size, usize... other_data_sizes>
    [[nodiscard]] constexpr auto
    join(const StaticString<first_data_size>& first, const StaticString<other_data_sizes>&... rest) const {
        if constexpr (sizeof...(rest) == 0) {
            return first;
        } else {
            return first + *this + join(rest...);
        }
    }

    [[nodiscard]] constexpr operator const char*() const {
        return c_str();
    }

    [[nodiscard]] constexpr operator std::string_view() const {
        return string_view();
    }

    [[nodiscard]] operator std::string() const {
        return string();
    }

    [[nodiscard]] operator std::filesystem::path() const {
        return string();
    }

    // make all template instantiations of this template a friend
    template<usize other_data_size>
    friend struct StaticString;
};
