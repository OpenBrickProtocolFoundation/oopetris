

#pragma once

#include "./client.hpp"

#include "./credentials/secret.hpp"
#include "./types.hpp"
#include "helper/export_symbols.hpp"

#include <core/helper/static_string.hpp>

namespace lobby {


    struct API {
    private:
        std::unique_ptr<oopetris::http::Client> m_client;
        std::optional<std::string> m_authentication_token;
        std::unique_ptr<secret::SecretStorage> m_secret_storage;

        // lobby commit used: https://github.com/OpenBrickProtocolFoundation/lobby/commit/2e0c8d05592f4e4d08437e6cb754a30f02c4e97c
        static constexpr StaticString supported_version{ "0.1.0" };

        [[nodiscard]] helper::expected<void, std::string> check_compatibility();

        [[nodiscard]] helper::expected<void, std::string> check_reachability();

        explicit API(ServiceProvider* service_provider, const std::string& api_url);

        helper::expected<lobby::VersionResult, std::string> get_version();

        helper::expected<lobby::LoginResponse, std::string> login(const lobby::Credentials& credentials);

    public:
        OOPETRIS_GRAPHICS_EXPORTED API(API&& other) noexcept;
        API& operator=(API&& other) noexcept = delete;

        API(const API& other) = delete;
        API& operator=(const API& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~API();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<API, std::string> static get_api(
                ServiceProvider* service_provider,
                const std::string& url
        );

        OOPETRIS_GRAPHICS_EXPORTED
        void static check_url(
                ServiceProvider* service_provider,
                const std::string& url,
                std::function<void(const bool success)>&& callback
        );

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool is_authenticated();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool authenticate(const Credentials& credentials);

        OOPETRIS_GRAPHICS_EXPORTED void logout();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<std::vector<LobbyInfo>, std::string> get_lobbies();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string> join_lobby(int lobby_id);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<LobbyDetail, std::string> get_lobby_detail(
                int lobby_id
        );

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string> delete_lobby(int lobby_id);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string> leave_lobby(int lobby_id);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string> start_lobby(int lobby_id);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<LobbyCreateResponse, std::string> create_lobby(
                const CreateLobbyRequest& arguments
        );

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<std::vector<PlayerInfo>, std::string> get_users();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string> register_user(
                const RegisterRequest& register_request
        );

    private:
        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED bool setup_authentication(const std::string& token);
    };


} // namespace lobby
