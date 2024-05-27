

#include "api.hpp"


namespace {


    inline helper::expected<std::string, std::string> is_json_response(const httplib::Result& result) {
        if (not result->has_header("Content-Type")) {
            return helper::unexpected<std::string>{ "Content-Type not set!" };
        }

        if (const auto value = result->get_header_value("Content-Type"); value != constants::json_content_type) {
            return helper::unexpected<std::string>{ fmt::format("Content-Type is not json but {}", value) };
        }

        return result->body;
    }


    inline helper::expected<lobby::ErrorResponse, std::string> is_error_message_response(const httplib::Result& result
    ) {

        const auto body = is_json_response(result);
        if (not body.has_value()) {
            return helper::unexpected<std::string>{ body.error() };
        }

        const auto parsed = json::try_parse_json<lobby::ErrorResponse>(body.value());

        if (parsed.has_value()) {
            return parsed.value();
        }

        return helper::unexpected<std::string>{ fmt::format("Couldn't parse json with error: {}", parsed.error()) };
    }

    inline helper::expected<void, std::string> is_request_ok(const httplib::Result& result, int ok_code = 200) {

        if (not result) {
            return helper::unexpected<std::string>{
                fmt::format("Request failed with: {}", httplib::to_string(result.error()))
            };
        }

        if (result->status == 401) {

            const auto error_type = is_error_message_response(result);

            if (error_type.has_value()) {
                return helper::unexpected<std::string>{ fmt::format("Unauthorized: {}", error_type.value().message) };
            }

            return helper::unexpected<std::string>{ "Unauthorized" };
        }


        if (result->status != ok_code) {

            const auto error_type = is_error_message_response(result);

            if (error_type.has_value()) {
                return helper::unexpected<std::string>{ fmt::format(
                        "Got error response with status code {}: '{}' and message: {}", result->status,
                        httplib::status_message(result->status), error_type.value().message
                ) };
            }


            return helper::unexpected<std::string>{ fmt::format(
                    "Got error response with status code {}: '{}' but expected {}", result->status,
                    httplib::status_message(result->status), ok_code
            ) };
        }

        return {};
    };


    template<typename T>
    helper::expected<T, std::string> get_json_from_request(const httplib::Result& result, int ok_code = 200) {

        const auto temp = is_request_ok(result, ok_code);
        if (not temp.has_value()) {
            return helper::unexpected<std::string>{ temp.error() };
        }

        const auto body = is_json_response(result);
        if (not body.has_value()) {
            return helper::unexpected<std::string>{ body.error() };
        }

        const auto parsed = json::try_parse_json<T>(body.value());

        if (parsed.has_value()) {
            return parsed.value();
        }

        return helper::unexpected<std::string>{ fmt::format("Couldn't parse json with error: {}", parsed.error()) };
    }


} // namespace


helper::expected<void, std::string> lobby::Client::check_compatibility() {
    const auto server_version = get_version();

    if (not server_version.has_value()) {
        return helper::unexpected<std::string>{ fmt::format(
                "Connecting to unsupported server, he can't report his version\nGot error: {}", server_version.error()
        ) };
    }

    const auto& version = server_version.value();

    //TODO(Totto):  if version is semver, support semver comparison
    if (Client::supported_version.string() != version.version) {
        return helper::unexpected<std::string>{ fmt::format(
                "Connecting to unsupported server, version is {}, but we support only {}",
                Client::supported_version.string(), version.version
        ) };
    }

    return {};
}

helper::expected<void, std::string> lobby::Client::check_reachability() {

    auto result = m_client.Get("/");

    if (not result) {
        return helper::unexpected<std::string>{
            fmt::format("Server not reachable: {}", httplib::to_string(result.error()))
        };
    }

    return {};
}

lobby::Client::Client(const std::string& api_url) : m_client{ api_url } {

    // clang-format off
            m_client.set_default_headers({
#if defined(CPPHTTPLIB_ZLIB_SUPPORT) || defined(CPPHTTPLIB_BROTLI_SUPPORT)
                    { "Accept-Encoding",

#if defined(CPPHTTPLIB_ZLIB_SUPPORT)
                        "gzip, deflate"
#endif
#if defined(CPPHTTPLIB_ZLIB_SUPPORT) && defined(CPPHTTPLIB_BROTLI_SUPPORT)
                        ", "
#endif
#if defined(CPPHTTPLIB_BROTLI_SUPPORT)
                        "br"
#endif
                        },
#endif
            // clang-format on
            { "Accept", constants::json_content_type } });

#if defined(CPPHTTPLIB_ZLIB_SUPPORT) || defined(CPPHTTPLIB_BROTLI_SUPPORT)
    m_client.set_compress(true);
    m_client.set_decompress(true);
#endif
}

helper::expected<lobby::VersionResult, std::string> lobby::Client::get_version() {
    auto res = m_client.Get("/version");

    return get_json_from_request<VersionResult>(res);
}


lobby::Client::Client(Client&& other) noexcept
    : m_client{ std::move(other.m_client) },
      m_authentication_token{ std::move(other.m_authentication_token) } { }

lobby::Client::~Client() = default;

helper::expected<lobby::Client, std::string> lobby::Client::get_client(const std::string& url) {

    Client client{ url };

    const auto reachable = client.check_reachability();

    if (not reachable.has_value()) {
        return helper::unexpected<std::string>{ reachable.error() };
    }

    //TODO(Totto):  once version is standard, check here if the version is supported

    return client;
}


helper::expected<lobby::LoginResponse, std::string> lobby::Client::login(const Credentials& credentials) {
    const auto json_result = json::try_json_to_string(credentials);
    if (not json_result.has_value()) {
        return helper::unexpected<std::string>{ json_result.error() };
    }

    auto res = m_client.Post("/login", json_result.value(), constants::json_content_type);

    return get_json_from_request<LoginResponse>(res);
}


bool lobby::Client::is_authenticated() {
    return m_authentication_token.has_value();
}

bool lobby::Client::authenticate(const Credentials& credentials) {

    const auto result = login(credentials);

    if (not result.has_value()) {
        spdlog::error("Failed authenticating user {}: {}", credentials.username, result.error());
        m_authentication_token = std::nullopt;
        return false;
    }

    m_authentication_token = result.value().jwt;

    m_client.set_bearer_token_auth(m_authentication_token.value());

    return true;
}

helper::expected<std::vector<lobby::LobbyInfo>, std::string> lobby::Client::get_lobbies() {
    auto res = m_client.Get("/lobbies");

    return get_json_from_request<std::vector<LobbyInfo>>(res);
}


helper::expected<void, std::string> lobby::Client::join_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client.Post(fmt::format("/lobbies/{}", lobby_id));

    return is_request_ok(res, 204);
}

helper::expected<lobby::LobbyDetail, std::string> lobby::Client::get_lobby_detail(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client.Get(fmt::format("/lobbies/{}", lobby_id));

    return get_json_from_request<LobbyDetail>(res);
}

helper::expected<void, std::string> lobby::Client::delete_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client.Delete(fmt::format("/lobbies/{}", lobby_id));

    return is_request_ok(res, 204);
}

helper::expected<void, std::string> lobby::Client::leave_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client.Put(fmt::format("/lobbies/{}/leave", lobby_id));

    return is_request_ok(res, 204);
}

helper::expected<void, std::string> lobby::Client::start_lobby(int lobby_id) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    auto res = m_client.Post(fmt::format("/lobbies/{}/start", lobby_id));

    return is_request_ok(res, 204);
}

helper::expected<lobby::LobbyCreateResponse, std::string> lobby::Client::create_lobby(
        const CreateLobbyRequest& arguments
) {
    if (not is_authenticated()) {
        return helper::unexpected<std::string>{
            "Authentication needed for this "
            "endpoint, but not authenticated!"
        };
    }

    const auto json_result = json::try_json_to_string(arguments);
    if (not json_result.has_value()) {
        return helper::unexpected<std::string>{ json_result.error() };
    }

    auto res = m_client.Post("/lobbies", json_result.value(), constants::json_content_type);

    return get_json_from_request<LobbyCreateResponse>(res, 201);
}

helper::expected<std::vector<lobby::PlayerInfo>, std::string> lobby::Client::get_users() {

    auto res = m_client.Get("/users");

    return get_json_from_request<std::vector<PlayerInfo>>(res);
}

helper::expected<void, std::string> lobby::Client::register_user(const RegisterRequest& register_request) {
    const auto json_result = json::try_json_to_string(register_request);
    if (not json_result.has_value()) {
        return helper::unexpected<std::string>{ json_result.error() };
    }

    auto res = m_client.Post("/register", json_result.value(), constants::json_content_type);

    return is_request_ok(res, 204);
}
