#pragma once


#include "helper/const_utils.hpp"
#include "helper/expected.hpp"
#include "helper/types.hpp"

#include <SDL.h>
#include <array>
#include <fmt/format.h>
#include <string>

namespace SDL {


    struct GUID {
    public:
        using ArrayType = std::array<u8, 16>;

    private:
        ArrayType m_guid;

    public:
        enum class FormatType { Long, Short };

        constexpr GUID() : m_guid{} { }
        constexpr GUID(const ArrayType& data) : m_guid{ data } { }

        GUID(const SDL_GUID& data);

        [[nodiscard]] static helper::expected<GUID, std::string> from_string(const std::string& value);

        [[nodiscard]] bool operator==(const GUID& other) const;

        [[nodiscard]] std::string to_string(FormatType type = FormatType::Long) const;
    };
} // namespace SDL


template<>
struct fmt::formatter<SDL::GUID> : formatter<std::string> {
    auto format(const SDL::GUID& guid, format_context& ctx) {
        return formatter<std::string>::format(guid.to_string(), ctx);
    }
};

namespace {

    // decode a single_hex_number
    [[nodiscard]] constexpr const_utils::expected<u8, std::string> single_hex_number(char n) {
        if (n >= '0' && n <= '9') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(n - '0'));
        }

        if (n >= 'A' && n <= 'F') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(n - 'A' + 10));
        }

        if (n >= 'a' && n <= 'f') {
            return const_utils::expected<u8, std::string>::good_result(static_cast<u8>(n - 'a' + 10));
        }

        return const_utils::expected<u8, std::string>::error_result("the input must be a valid hex character");
    }

    // decode a single 2 digit color value in hex
    [[nodiscard]] constexpr const_utils::expected<u8, std::string> single_hex_color_value(const char* input) {

        const auto first = single_hex_number(input[0]);

        PROPAGATE(first, u8, std::string);

        const auto second = single_hex_number(input[1]);

        PROPAGATE(second, u8, std::string);

        return const_utils::expected<u8, std::string>::good_result((first.value() << 4) | second.value());
    }

    [[nodiscard]] constexpr const_utils::expected<SDL::GUID, std::string>
    get_guid_from_string_impl(const char* input, std::size_t size) {

        if (size == 0) {
            return const_utils::expected<SDL::GUID, std::string>::error_result(
                    "not enough data to determine the literal type"
            );
        }

        constexpr std::size_t amount = 16;

        size_t width = 2;

        if (size == amount * 2) {
            width = 2;
        } else if (size == (amount * 2 + (amount - 1))) {
            width = 3;
        } else {

            return const_utils::expected<SDL::GUID, std::string>::error_result("Unrecognized guid literal");
        }


        SDL::GUID::ArrayType result{};

        for (size_t i = 0; i < amount; ++i) {
            size_t offset = i * (width);


            const auto temp_result =
                    single_hex_color_value(input + offset); //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

            PROPAGATE(temp_result, SDL::GUID, std::string);

            const auto value = temp_result.value();

            result.at(i) = value;
        }

        return const_utils::expected<SDL::GUID, std::string>::good_result(SDL::GUID{ result });
    }

} // namespace


namespace detail {

    [[nodiscard]] constexpr const_utils::expected<SDL::GUID, std::string> get_guid_from_string(const std::string& input
    ) {
        return get_guid_from_string_impl(input.c_str(), input.size());
    }

} // namespace detail


consteval SDL::GUID operator""_guid(const char* input, std::size_t size) {
    const auto result = get_guid_from_string_impl(input, size);

    CONSTEVAL_STATIC_ASSERT(result.has_value(), "incorrect guid literal");

    return result.value();
}
