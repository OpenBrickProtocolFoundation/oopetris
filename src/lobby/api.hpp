

#pragma once


#if defined(_HTTPLIB_NOT_SUPPORTED)
//TODO: nintendo switch doesn't support httplib, but it supports curl (which is not that convenient)
#include <curl/curl.h>
#else

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#define CPPHTTPLIB_USE_POLL // NOLINT(cppcoreguidelines-macro-usage)

#include <httplib.h>

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

#include "helper/static_string.hpp"
#include "lobby/types.hpp"

//TODO: cleanup this mess in different header files
#if !defined(_HTTPLIB_NOT_SUPPORTED)

namespace constants {
    const std::string json_content_type = "application/json";
}

namespace {


    inline tl::expected<std::string, std::string> is_json_response(const httplib::Result& result) {
        if (not result->has_header("Content-Type")) {
            return tl::make_unexpected("Content-Type not set!");
        }

        if (const auto value = result->get_header_value("Content-Type"); value != constants::json_content_type) {
            return tl::make_unexpected(fmt::format("Content-Type is not json but {}", value));
        }

        return result->body;
    }


    inline tl::expected<lobby::ErrorResponse, std::string> is_error_message_response(const httplib::Result& result) {

        const auto body = is_json_response(result);
        if (not body.has_value()) {
            return tl::make_unexpected(body.error());
        }

        const auto parsed = json::try_parse_json<lobby::ErrorResponse>(body.value());

        if (parsed.has_value()) {
            return parsed.value();
        }

        return tl::make_unexpected(fmt::format("Couldn't parse json with error: {}", parsed.error()));
    }

    inline tl::expected<bool, std::string> is_request_ok(const httplib::Result& result, int ok_code = 200) {

        if (not result) {
            return tl::make_unexpected(fmt::format("Request failed with: {}", httplib::to_string(result.error())));
        }

        if (result->status == 401) {

            const auto error_type = is_error_message_response(result);

            if (error_type.has_value()) {
                return tl::make_unexpected(fmt::format("Unauthorized: {}", error_type.value().message));
            }

            return tl::make_unexpected("Unauthorized");
        }


        if (result->status != ok_code) {

            const auto error_type = is_error_message_response(result);

            if (error_type.has_value()) {
                return tl::make_unexpected(fmt::format(
                        "Got error response with status code {}: '{}' and message: {}", result->status,
                        httplib::status_message(result->status), error_type.value().message
                ));
            }


            return tl::make_unexpected(fmt::format(
                    "Got error response with status code {}: '{}' but expected {}", result->status,
                    httplib::status_message(result->status), ok_code
            ));
        }

        return true;
    };


    template<typename T>
    tl::expected<T, std::string> get_json_from_request(const httplib::Result& result, int ok_code = 200) {

        const auto temp = is_request_ok(result, ok_code);
        if (not temp.has_value()) {
            return tl::make_unexpected(temp.error());
        }

        const auto body = is_json_response(result);
        if (not body.has_value()) {
            return tl::make_unexpected(body.error());
        }

        const auto parsed = json::try_parse_json<T>(body.value());

        if (parsed.has_value()) {
            return parsed.value();
        }

        return tl::make_unexpected(fmt::format("Couldn't parse json with error: {}", parsed.error()));
    }


} // namespace


namespace lobby {


    struct Client {
    private:
        httplib::Client m_client;
        std::string authentication_token{};

        // lobby commit used: https://github.com/OpenBrickProtocolFoundation/lobby/commit/2e0c8d05592f4e4d08437e6cb754a30f02c4e97c
        static constexpr StaticString supported_version{ "0.1.0" };

        tl::expected<bool, std::string> check_compatibility() {
            const auto server_version = get_version();

            if (not server_version.has_value()) {
                return tl::make_unexpected(fmt::format(
                        "Connecting to unsupported server, he can't report his version\nGot error: {}",
                        server_version.error()
                ));
            }

            const auto& version = server_version.value();

            //TODO: if version is semver, support semver comparison
            if (Client::supported_version.string() != version.version) {
                return tl::make_unexpected(fmt::format(
                        "Connecting to unsupported server, version is {}, but we support only {}",
                        Client::supported_version.string(), version.version
                ));
            }

            return true;
        }

        tl::expected<bool, std::string> check_reachability() {

            auto result = m_client.Get("/");

            if (not result) {
                return tl::make_unexpected(fmt::format("Server not reachable: {}", httplib::to_string(result.error())));
            }

            return true;
        }

        explicit Client(const std::string& api_url) : m_client{ api_url } {


            m_client.set_default_headers({
#if defined(CPPHTTPLIB_ZLIB_SUPPORT)
                { "Accept-Encoding", "gzip, deflate" },
#endif
                {
                    "Accept", constants::json_content_type
                }
            });

#if defined(CPPHTTPLIB_ZLIB_SUPPORT)
            m_client.set_compress(true);
#endif
        }

        tl::expected<VersionResult, std::string> get_version() {
            auto res = m_client.Get("/version");

            return get_json_from_request<VersionResult>(res);
        }

    public:
        Client(Client&& other)
            : m_client{ std::move(other.m_client) },
              authentication_token{ std::move(other.authentication_token) } { }

        tl::expected<Client, std::string> static get_client(const std::string& url) {

            Client client{ url };

            const auto reachable = client.check_reachability();

            if (not reachable.has_value()) {
                return tl::make_unexpected(reachable.error());
            }

            //TODO: once version is standard, check here if the version is supported

            return client;
        }

    private:
        tl::expected<LoginResponse, std::string> login(const Credentials& credentials) {
            const auto json_result = json::try_json_to_string(credentials);
            if (not json_result.has_value()) {
                return tl::make_unexpected(json_result.error());
            }

            auto res = m_client.Post("/login", json_result.value(), constants::json_content_type);

            return get_json_from_request<LoginResponse>(res);
        }


    public:
        bool is_authenticated() {
            return not authentication_token.empty();
        }

        bool authenticate(const Credentials& credentials) {

            const auto result = login(credentials);

            if (not result.has_value()) {
                spdlog::error("Failed authenticating user {}: {}", credentials.username, result.error());
                authentication_token = "";
                return false;
            }

            authentication_token = result.value().jwt;

            m_client.set_bearer_token_auth(authentication_token);

            return true;
        }

        tl::expected<std::vector<LobbyInfo>, std::string> get_lobbies() {
            auto res = m_client.Get("/lobbies");

            return get_json_from_request<std::vector<LobbyInfo>>(res);
        }


        tl::expected<bool, std::string> join_lobby(int lobby_id) {
            if (not is_authenticated()) {
                return tl::make_unexpected("Authentication needed for this endpoint, but not authenticated!");
            }

            auto res = m_client.Post(fmt::format("/lobbies/{}", lobby_id));

            return is_request_ok(res, 204);
        }

        tl::expected<LobbyDetail, std::string> get_lobby_detail(int lobby_id) {
            if (not is_authenticated()) {
                return tl::make_unexpected("Authentication needed for this endpoint, but not authenticated!");
            }

            auto res = m_client.Get(fmt::format("/lobbies/{}", lobby_id));

            return get_json_from_request<LobbyDetail>(res);
        }

        tl::expected<bool, std::string> delete_lobby(int lobby_id) {
            if (not is_authenticated()) {
                return tl::make_unexpected("Authentication needed for this endpoint, but not authenticated!");
            }

            auto res = m_client.Delete(fmt::format("/lobbies/{}", lobby_id));

            return is_request_ok(res, 204);
        }

        tl::expected<bool, std::string> leave_lobby(int lobby_id) {
            if (not is_authenticated()) {
                return tl::make_unexpected("Authentication needed for this endpoint, but not authenticated!");
            }

            auto res = m_client.Put(fmt::format("/lobbies/{}/leave", lobby_id));

            return is_request_ok(res, 204);
        }

        tl::expected<bool, std::string> start_lobby(int lobby_id) {
            if (not is_authenticated()) {
                return tl::make_unexpected("Authentication needed for this endpoint, but not authenticated!");
            }

            auto res = m_client.Post(fmt::format("/lobbies/{}/start", lobby_id));

            return is_request_ok(res, 204);
        }

        tl::expected<LobbyCreateResponse, std::string> create_lobby(const CreateLobbyRequest& arguments) {
            if (not is_authenticated()) {
                return tl::make_unexpected("Authentication needed for this endpoint, but not authenticated!");
            }

            const auto json_result = json::try_json_to_string(arguments);
            if (not json_result.has_value()) {
                return tl::make_unexpected(json_result.error());
            }

            auto res = m_client.Post("/lobbies", json_result.value(), constants::json_content_type);

            return get_json_from_request<LobbyCreateResponse>(res, 201);
        }

        tl::expected<std::vector<PlayerInfo>, std::string> get_users() {

            auto res = m_client.Get("/users");

            return get_json_from_request<std::vector<PlayerInfo>>(res);
        }

        tl::expected<bool, std::string> register_user(const RegisterRequest& register_request) {
            const auto json_result = json::try_json_to_string(register_request);
            if (not json_result.has_value()) {
                return tl::make_unexpected(json_result.error());
            }

            auto res = m_client.Post("/register", json_result.value(), constants::json_content_type);

            return is_request_ok(res, 204);
        }
    };


} // namespace lobby

#else

//TODO: this is just a dummy atm

namespace lobby {


    struct Client {

        Client(Client&&) { }

        tl::expected<Client, std::string> static get_client([[maybe_unused]] const std::string& url) {

            return tl::make_unexpected("NOT SUPPORTED ATM");
        }
    };
} // namespace lobby


#endif
