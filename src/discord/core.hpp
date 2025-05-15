

#pragma once

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>

#include "../helper/windows.hpp"

#include <chrono>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif


#include <discordpp.h>
#include <string>

namespace constants::discord {
    constexpr u64 application_id = 1220147916371394650ULL;


//TODO(Totto):  this isn't correct for all platforms and needs to be tested
#if defined(__ANDROID__)
#error "Not supported"
#elif defined(__CONSOLE__)
#error "Not supported"
#elif defined(FLATPAK_BUILD)
    constexpr const char* platform_dependent_launch_arguments =
            "flatpak run io.github.openbrickprotocolfoundation.oopetris --discord";
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    constexpr const char* platform_dependent_launch_arguments = "oopetris.exe --discord";
#elif defined(__APPLE__)
    constexpr const char* platform_dependent_launch_arguments = "TODO";
#elif defined(__linux__)
    constexpr const char* platform_dependent_launch_arguments = "oopetris --discord";
#else
#error "Unsupported platform"
#endif

#if defined(__ANDROID__)
    constexpr const discordpp::ActivityGamePlatforms supported_platforms = discordpp::ActivityGamePlatforms::Android;
#elif defined(__CONSOLE__)
#error "Not supported"
#elif defined(FLATPAK_BUILD) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) \
        || defined(__APPLE__) || defined(__linux__)
    constexpr const discordpp::ActivityGamePlatforms supported_platforms = discordpp::ActivityGamePlatforms::Desktop;
#else
#error "Unsupported platform"
#endif


    // manually synchronized to https://discord.com/developers/applications/1220147916371394650/rich-presence/assets
    enum class ArtAsset : u8 { Logo };

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string get_asset_key(ArtAsset asset);

} // namespace constants::discord


struct DiscordActivityWrapper {
private:
    discordpp::Activity m_activity;

    [[nodiscard]] discordpp::ActivityTimestamps get_timestamps();

    [[nodiscard]] discordpp::ActivityAssets get_assets();

public:
    //TODO(Totto):  Add support for party and invites / join / invitations / spectate

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper(const std::string& details, discordpp::ActivityTypes type);

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper&
    set_large_image(const std::string& text, constants::discord::ArtAsset asset);

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper&
    set_small_image(const std::string& text, constants::discord::ArtAsset asset);

    OOPETRIS_GRAPHICS_EXPORTED DiscordActivityWrapper& set_details(const std::string& text);


    template<typename T>
    DiscordActivityWrapper& set_start_timestamp(const std::chrono::time_point<T>& point) {

        const auto seconds_since_epoch =
                static_cast<u64>(std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch()).count()
                );

        auto timestamps = this->get_timestamps();

        timestamps.SetStart(seconds_since_epoch);
        m_activity.SetTimestamps(timestamps);

        return *this;
    }

    template<typename T>
    DiscordActivityWrapper& set_end_timestamp(const std::chrono::time_point<T>& point) {

        const auto seconds_since_epoch =
                static_cast<u64>(std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch()).count()
                );

        auto timestamps = this->get_timestamps();

        timestamps.SetEnd(seconds_since_epoch);
        m_activity.SetTimestamps(timestamps);

        return *this;
    }


    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] const discordpp::Activity& get_raw() const;
};

enum class DiscordStatus : u8 { Starting = 0, Ok, Error };

struct DiscordInstance {
private:
    discordpp::Client m_client;
    discordpp::UserHandle m_current_user;
    DiscordStatus m_status;


public:
    OOPETRIS_GRAPHICS_EXPORTED explicit DiscordInstance();

    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] DiscordStatus get_status();

    OOPETRIS_GRAPHICS_EXPORTED DiscordInstance(DiscordInstance&& old) noexcept;

    OOPETRIS_GRAPHICS_EXPORTED DiscordInstance& operator=(DiscordInstance&& other) noexcept;

    DiscordInstance(DiscordInstance& old) noexcept = delete;

    DiscordInstance& operator=(const DiscordInstance& other) noexcept = delete;

    OOPETRIS_GRAPHICS_EXPORTED ~DiscordInstance();

    OOPETRIS_GRAPHICS_EXPORTED static void update();

    OOPETRIS_GRAPHICS_EXPORTED void set_activity(const DiscordActivityWrapper& activity);

private:
    void after_ready();
    void clear_activity();
};
