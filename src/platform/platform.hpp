

#pragma once

#include "helper/parse_json.hpp"
#include "helper/types.hpp"

enum class Platform : u8 { PC, Android, Console };


NLOHMANN_JSON_SERIALIZE_ENUM( // NOLINT(modernize-type-traits,cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
        Platform,
        {
                {     Platform::PC,      "pc"},
                {Platform::Android, "android"},
                {Platform::Console, "console"}
}
)


namespace utils {

    constexpr std::string get_platform() {

#if defined(__ANDROID__)
        return "android";
#elif defined(__CONSOLE__)
        return "console";
#else
        return "pc";
#endif
    };

} // namespace utils
