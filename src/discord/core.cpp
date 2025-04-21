
#include <core/helper/utils.hpp>

#include "./core.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>


[[nodiscard]] std::string constants::discord ::get_asset_key(constants::discord::ArtAsset asset) {

    switch (asset) {
        case ArtAsset::Logo:
            return "logo";
        default:
            UNREACHABLE();
    }
}

DiscordInstance::DiscordInstance()
    : m_current_user{ discordpp::UserHandle::nullobj },
      m_status{ DiscordStatus::Starting } {

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


    m_client.SetStatusChangedCallback(
            [this](discordpp::Client::Status status, discordpp::Client::Error error, int32_t error_detail) -> void {
                if (error != discordpp::Client::Error::None) {
                    this->m_status = DiscordStatus::Error;
                    spdlog::error(
                            "Connection Error: {} - Details: {}", discordpp::Client::ErrorToString(error), error_detail
                    );
                    return;
                }

                if (status == discordpp::Client::Status::Ready) {
                    this->m_status = DiscordStatus::Ok;
                    this->after_ready();
                    return;
                }
            }
    );

    m_client.SetApplicationId(constants::discord::application_id);

    m_client.Connect();
}

void DiscordInstance::after_ready() {

    this->m_client.GetDiscordClientConnectedUser(
            constants::discord::application_id,
            [this](const discordpp::ClientResult& result, std::optional<discordpp::UserHandle> user) -> void {
                if (result.Successful() and user.has_value()) {

                    auto user_handle = m_client.GetUser(user->Id());
                    if (not user_handle.has_value()) {
                        spdlog::error("Current Connected User Error: Can't get userhandle from id: {}", user->Id());

                        return;
                    }

                    this->m_current_user = user_handle.value();
                    spdlog::info("Current user updated: {}", user_handle->Username());

                    return;
                }

                spdlog::error("Current Connected User Error: {}", result.ToString());
            }
    );
}


DiscordInstance::DiscordInstance(DiscordInstance&& old) noexcept
    : m_client{ std::move(old.m_client) },
      m_current_user{ std::move(old.m_current_user) },
      m_status{ old.m_status } {
    old.m_client = discordpp::Client{};
    old.m_current_user = discordpp::UserHandle::nullobj;
    old.m_status = DiscordStatus::Error;
}


DiscordInstance& DiscordInstance::operator=(DiscordInstance&& other) noexcept {
    if (this != &other) {

        m_client = std::move(other.m_client);
        m_current_user = std::move(other.m_current_user);
        m_status = other.m_status;

        other.m_client = discordpp::Client{};
        other.m_current_user = discordpp::UserHandle::nullobj;
        other.m_status = DiscordStatus::Error;
    }
    return *this;
};

DiscordInstance::~DiscordInstance() {
    if (m_client.operator bool()) {
        clear_activity();
        m_client.Disconnect();
    }
}

[[nodiscard]] DiscordStatus DiscordInstance::get_status() {
    return m_status;
}

void DiscordInstance::update() {
    discordpp::RunCallbacks();
}


void DiscordInstance::set_activity(const DiscordActivityWrapper& activity) {

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
            spdlog::error("Rich Presence update failed: {}", result.ToString());
        }
    });
}


void DiscordInstance::clear_activity() {
    m_client.ClearRichPresence();
}


DiscordActivityWrapper::DiscordActivityWrapper(const std::string& details, discordpp::ActivityTypes type) {
    // NOTE: this are partial fields, that are set by the final call, do not set them manually
    // https://discord.com/developers/docs/rich-presence/using-with-the-game-sdk#partial-activity-struct
    // m_activity.SetName(constants::program_name);
    // m_activity.SetApplicationId(constants::application_id);

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


[[nodiscard]] const discordpp::Activity& DiscordActivityWrapper::get_raw() const {
    return m_activity;
}


discordpp::ActivityTimestamps DiscordActivityWrapper::get_timestamps() {

    std::optional<discordpp::ActivityTimestamps> timestamps = this->m_activity.Timestamps();

    if (timestamps.has_value()) {
        return timestamps.value();
    }

    return discordpp::ActivityTimestamps::nullobj;
}


[[nodiscard]] discordpp::ActivityAssets DiscordActivityWrapper::get_assets() {
    std::optional<discordpp::ActivityAssets> assets = this->m_activity.Assets();

    if (assets.has_value()) {
        return assets.value();
    }

    return discordpp::ActivityAssets::nullobj;
}
