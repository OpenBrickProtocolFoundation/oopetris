

#include "api.hpp"

#include <spdlog/spdlog.h>


#if defined(_OOPETRIS_ONLINE_USE_CURL)
#include "./curl_client.hpp"
#else
#include "./httplib_client.hpp"
#endif


helper::expected<void, std::string> lobby::API::check_compatibility() {
    const auto server_version = get_version();

    if (not server_version.has_value()) {
        return helper::unexpected<std::string>{ fmt::format(
                "Connecting to unsupported server, he can't report his version\nGot error: {}", server_version.error()
        ) };
    }

    const auto& version = server_version.value();

    //TODO(Totto):  if version is semver, support semver comparison
    if (API::supported_version.string() != version.version) {
        return helper::unexpected<std::string>{ fmt::format(
                "Connecting to unsupported server, version is {}, but we support only {}",
                API::supported_version.string(), version.version
        ) };
    }

    return {};
}

helper::expected<void, std::string> lobby::API::check_reachability() {

    auto result = m_client->Get("/");

    if (auto error = result->get_error(); error.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("Server not reachable: {}", error.value()) };
    }

    return {};
}

lobby::API::API(const std::string& api_url)
    : m_client{ std::make_unique<oopetris::http::implementation::ActualClient>(api_url) } { }

helper::expected<lobby::VersionResult, std::string> lobby::API::get_version() {
    auto res = m_client->Get("/version");

    return get_json_from_request<VersionResult>(res);
}


lobby::API::API(API&& other) noexcept
    : m_client{ std::move(other.m_client) },
      m_authentication_token{ std::move(other.m_authentication_token) } { }

lobby::API::~API() = default;

helper::expected<lobby::API, std::string> lobby::API::get_api(const std::string& url) {

    API api{ url };

    const auto reachable = api.check_reachability();

    if (not reachable.has_value()) {
        return helper::unexpected<std::string>{ reachable.error() };
    }

    //TODO(Totto):  once version is standard, check here if the version is supported

    return api;
}


helper::expected<lobby::LoginResponse, std::string> lobby::API::login(const Credentials& credentials) {
    auto json_result = json::try_json_to_string(credentials);
    if (not json_result.has_value()) {
        return helper::unexpected<std::string>{ json_result.error() };
    }

    auto payload = std::make_pair<std::string, std::string>(
            std::move(json_result.value()), ::http::constants::json_content_type
    );

    auto res = m_client->Post("/login", payload);

    return get_json_from_request<LoginResponse>(res);
}


bool lobby::API::is_authenticated() {
    return m_authentication_token.has_value();
}

bool lobby::API::authenticate(const Credentials& credentials) {

    const auto result = login(credentials);

    if (not result.has_value()) {
        spdlog::error("Failed authenticating user {}: {}", credentials.username, result.error());
        m_authentication_token = std::nullopt;
        return false;
    }

    m_authentication_token = result.value().jwt;

    m_client->SetBearerAuth(m_authentication_token.value());

    return true;
}

helper::expected<std::vector<lobby::LobbyInfo>, std::string> lobby::API::get_lobbies() {
    auto res = m_client->Get("/lobbies");

    return get_json_from_request<std::vector<LobbyInfo>>(res);
}


helper::expected<void, std::string> lobby::API::join_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client->Post(fmt::format("/lobbies/{}", lobby_id), std::nullopt);

    return is_request_ok(res, 204);
}

helper::expected<lobby::LobbyDetail, std::string> lobby::API::get_lobby_detail(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client->Get(fmt::format("/lobbies/{}", lobby_id));

    return get_json_from_request<LobbyDetail>(res);
}

helper::expected<void, std::string> lobby::API::delete_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client->Delete(fmt::format("/lobbies/{}", lobby_id));

    return is_request_ok(res, 204);
}

helper::expected<void, std::string> lobby::API::leave_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client->Put(fmt::format("/lobbies/{}/leave", lobby_id), std::nullopt);

    return is_request_ok(res, 204);
}

helper::expected<void, std::string> lobby::API::start_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client->Post(fmt::format("/lobbies/{}/start", lobby_id), std::nullopt);

    return is_request_ok(res, 204);
}

helper::expected<lobby::LobbyCreateResponse, std::string> lobby::API::create_lobby(const CreateLobbyRequest& arguments
) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto json_result = json::try_json_to_string(arguments);
    if (not json_result.has_value()) {
        return helper::unexpected<std::string>{ json_result.error() };
    }

    auto payload = std::make_pair<std::string, std::string>(
            std::move(json_result.value()), ::http::constants::json_content_type
    );

    auto res = m_client->Post("/lobbies", payload);

    return get_json_from_request<LobbyCreateResponse>(res, 201);
}

helper::expected<std::vector<lobby::PlayerInfo>, std::string> lobby::API::get_users() {

    auto res = m_client->Get("/users");

    return get_json_from_request<std::vector<PlayerInfo>>(res);
}

helper::expected<void, std::string> lobby::API::register_user(const RegisterRequest& register_request) {
    auto json_result = json::try_json_to_string(register_request);
    if (not json_result.has_value()) {
        return helper::unexpected<std::string>{ json_result.error() };
    }

    auto payload = std::make_pair<std::string, std::string>(
            std::move(json_result.value()), ::http::constants::json_content_type
    );

    auto res = m_client->Post("/register", payload);

    return is_request_ok(res, 204);
}
