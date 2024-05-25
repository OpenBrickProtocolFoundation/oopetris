
#include <core/helper/utils.hpp>

#include "guid.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

sdl::GUID::GUID(const SDL_GUID& data) : m_guid{} {
    std::copy(std::begin(data.data), std::end(data.data), std::begin(m_guid));
}

[[nodiscard]] helper::expected<sdl::GUID, std::string> sdl::GUID::from_string(const std::string& value) {

    const auto result = detail::get_guid_from_string(value);

    if (result.has_value()) {
        return result.value();
    }

    return helper::unexpected<std::string>{ result.error() };
}

[[nodiscard]] bool sdl::GUID::operator==(const GUID& other) const {
    return m_guid == other.m_guid;
}


[[nodiscard]] std::string sdl::GUID::to_string(FormatType type) const {
    switch (type) {
        case FormatType::Long: {
            return fmt::format("{:02x}", fmt::join(m_guid, ":"));
        }
        case FormatType::Short: {
            return fmt::format("{:02x}", fmt::join(m_guid, ""));
        }
        default:
            UNREACHABLE();
    }
}
