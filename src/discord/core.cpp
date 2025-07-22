
#include <core/helper/utils.hpp>

#include "../helper/spdlog_wrapper.hpp"
#include "./core.hpp"

#include <fmt/format.h>

namespace {

    [[nodiscard]] std::string result_to_string(const discordpp::ClientResult& result) {
        const auto error_type = result.Type();

        if (error_type == discordpp::ErrorType::None) {
            return "None";
        }

        if (error_type == discordpp::ErrorType::HTTPError) {
            return fmt::format(
                    "ErrorType: {} Message: {} Status: {} ErrorCode: {}", discordpp::EnumToString(error_type),
                    result.Error(), discordpp::EnumToString(result.Status()), result.ErrorCode()
            );
        }

        return fmt::format("ErrorType: {} Message: {}", discordpp::EnumToString(error_type), result.Error());
    }

} // namespace


[[nodiscard]] std::string constants::discord ::get_asset_key(constants::discord::ArtAsset asset) {

    switch (asset) {
        case ArtAsset::Logo:
            return "logo";
        default:
            UNREACHABLE();
    }
}


DiscordInstance::DiscordInstance() : m_current_user{ discordpp::UserHandle::nullobj } {

    m_client.SetApplicationId(constants::discord::application_id);

    m_client.AddLogCallback(
            [](std::string message, discordpp::LoggingSeverity severity) -> void {
                switch (severity) {
                    case discordpp::LoggingSeverity::Error:
                        spdlog::error("DISCORD SDK: {}", message);
                        break;
                    case discordpp::LoggingSeverity::Warning:
                        spdlog::warn("DISCORD SDK: {}", message);
                        break;
                    case discordpp::LoggingSeverity::Info:
                        spdlog::info("DISCORD SDK: {}", message);
                        break;
                    case discordpp::LoggingSeverity::Verbose:
                        spdlog::debug("DISCORD SDK: {}", message);
                        break;
                    case discordpp::LoggingSeverity::None:
                        break;
                }
            },
#if !defined(NDEBUG)
            discordpp::LoggingSeverity::Verbose
#else
            discordpp::LoggingSeverity::Error
#endif
    );

    after_ready();
}

void DiscordInstance::after_ready() {

    this->m_client.GetDiscordClientConnectedUser(
            constants::discord::application_id,
            [this](const discordpp::ClientResult& result, std::optional<discordpp::UserHandle> user) -> void {
                if (result.Successful() and user.has_value()) {

                    this->m_current_user = user.value();
                    spdlog::info("Current user updated: {}", user->Username());

                    return;
                }

                spdlog::error("Current Connected User Error: {}", result_to_string(result));
            }
    );

    auto result = m_client.RegisterLaunchCommand(
            constants::discord::application_id, constants::discord::platform_dependent_launch_arguments
    );
    if (not result) {
        spdlog::warn("Discord: Failed to Register Launch Command");
    }
}


DiscordInstance::DiscordInstance(DiscordInstance&& old) noexcept
    : m_client{ std::move(old.m_client) },
      m_current_user{ std::move(old.m_current_user) } {
    old.m_client = discordpp::Client{};
    old.m_current_user = discordpp::UserHandle::nullobj;
}


DiscordInstance& DiscordInstance::operator=(DiscordInstance&& other) noexcept {
    if (this != &other) {

        m_client = std::move(other.m_client);
        m_current_user = std::move(other.m_current_user);

        other.m_client = discordpp::Client{};
        other.m_current_user = discordpp::UserHandle::nullobj;
    }
    return *this;
};

DiscordInstance::~DiscordInstance() {
    if (m_client.operator bool()) {
        clear_activity();
    }
}

void DiscordInstance::update() {
    discordpp::RunCallbacks();
}


void DiscordInstance::set_activity(DiscordActivityWrapper activity) {


    const auto& raw_activity = activity.get_raw();

    if (not raw_activity.operator bool()) {
        spdlog::error("Tried to set an invalid Discord Activity!");
        return;
    }

    // Update rich presence
    m_client.UpdateRichPresence(raw_activity, [](const discordpp::ClientResult& result) {
        if (result.Successful()) {
            spdlog::info("Rich Presence updated successfully");
        } else {
            spdlog::error("Rich Presence update failed: {}", result_to_string(result));
        }
    });
}


void DiscordInstance::clear_activity() {
    m_client.ClearRichPresence();
}

DiscordActivityWrapper::DiscordActivityWrapper(DiscordActivityWrapper&& old) noexcept
    : m_activity{ std::move(old.m_activity) } {
    old.m_activity = discordpp::Activity::nullobj;
}


DiscordActivityWrapper& DiscordActivityWrapper::operator=(DiscordActivityWrapper&& other) noexcept {
    if (this != &other) {

        m_activity = std::move(other.m_activity);

        other.m_activity = discordpp::Activity::nullobj;
    }
    return *this;
};

DiscordActivityWrapper::DiscordActivityWrapper(const std::string& details, discordpp::ActivityTypes type) {
    // NOTE: this are partial fields, that are set by the final call, do not set them manually
    // https://discord.com/developers/docs/rich-presence/using-with-the-game-sdk#partial-activity-struct
    // m_activity.SetName(constants::program_name.c_str());
    // m_activity.SetApplicationId(constants::discord::application_id);

    m_activity.SetDetails(details);
    m_activity.SetType(type);
    m_activity.SetSupportedPlatforms(constants::discord::supported_platforms);
}


DiscordActivityWrapper&
DiscordActivityWrapper::set_large_image(const std::string& text, constants::discord::ArtAsset asset) {
    auto assets = this->get_assets();

    const auto asset_key = constants::discord::get_asset_key(asset);

    assets.SetLargeImage(asset_key);
    assets.SetLargeText(text);

    m_activity.SetAssets(assets);

    return *this;
}


DiscordActivityWrapper&
DiscordActivityWrapper::set_small_image(const std::string& text, constants::discord::ArtAsset asset) {
    auto assets = this->get_assets();

    const auto asset_key = constants::discord::get_asset_key(asset);

    assets.SetSmallImage(asset_key);
    assets.SetSmallText(text);

    m_activity.SetAssets(assets);

    return *this;
}

DiscordActivityWrapper& DiscordActivityWrapper::set_details(const std::string& text) {
    m_activity.SetState(text);

    return *this;
}


DiscordActivityWrapper DiscordActivityWrapper::build() {

    return std::move(*this);
}

[[nodiscard]] const discordpp::Activity& DiscordActivityWrapper::get_raw() const {
    return m_activity;
}


discordpp::ActivityTimestamps DiscordActivityWrapper::get_timestamps() {

    std::optional<discordpp::ActivityTimestamps> timestamps = this->m_activity.Timestamps();

    if (timestamps.has_value()) {
        return timestamps.value();
    }

    return discordpp::ActivityTimestamps();
}


[[nodiscard]] discordpp::ActivityAssets DiscordActivityWrapper::get_assets() {
    std::optional<discordpp::ActivityAssets> assets = this->m_activity.Assets();

    if (assets.has_value()) {
        return assets.value();
    }

    return discordpp::ActivityAssets();
}
