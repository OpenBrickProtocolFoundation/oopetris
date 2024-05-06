
#include "guid.hpp"
#include "helper/utils.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

SDL::GUID::GUID(const SDL_GUID& data) : m_guid{} {
    std::copy(std::begin(data.data), std::end(data.data), std::begin(m_guid));
}

[[nodiscard]] helper::expected<SDL::GUID, std::string> SDL::GUID::from_string(const std::string& value) {

    const auto result = detail::get_guid_from_string(value);

    if (result.has_value()) {
        return result.value();
    }

    return helper::unexpected<std::string>{ result.error() };
}


[[nodiscard]] std::string SDL::GUID::to_string(FormatType type) const {
    switch (type) {
        case FormatType::Long: {
            return fmt::format("{:02x}", fmt::join(m_guid, ":"));
        }
        case FormatType::Short: {
            return fmt::format("{:02x}", fmt::join(m_guid, ""));
        }
        default:
            utils::unreachable();
    }
}
