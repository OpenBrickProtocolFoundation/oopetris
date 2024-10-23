

#pragma once

#include "./client.hpp"

#include "./types.hpp"
#include "helper/windows.hpp"

#include <core/helper/static_string.hpp>

namespace lobby {


    struct API {
    private:
        std::unique_ptr<oopetris::http::Client> m_client;
        std::optional<std::string> m_authentication_token;

        // lobby commit used: https://github.com/OpenBrickProtocolFoundation/lobby/commit/2e0c8d05592f4e4d08437e6cb754a30f02c4e97c
        static constexpr StaticString supported_version{ "0.1.0" };

        [[nodiscard]] helper::expected<void, std::string> check_compatibility();

        [[nodiscard]] helper::expected<void, std::string> check_reachability();

        explicit API(const std::string& api_url);

        helper::expected<lobby::VersionResult, std::string> get_version();

        helper::expected<lobby::LoginResponse, std::string> login(const lobby::Credentials& credentials);


    public:
        OOPETRIS_GRAPHICS_EXPORTED API(API&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED API& operator=(API&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED API(const API& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED API& operator=(const API& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~API();

        OOPETRIS_GRAPHICS_EXPORTED
        [[nodiscard]] helper::expected<API, std::string> static get_api(const std::string& url);


        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool is_authenticated();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] bool authenticate(const Credentials& credentials);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<std::vector<LobbyInfo>, std::string> get_lobbies();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<void, std::string> join_lobby(int lobby_id);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<LobbyDetail, std::string> get_lobby_detail(
                int lobby_id
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<void, std::string> delete_lobby(int lobby_id);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<void, std::string> leave_lobby(int lobby_id);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<void, std::string> start_lobby(int lobby_id);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<LobbyCreateResponse, std::string> create_lobby(
                const CreateLobbyRequest& arguments
        );

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<std::vector<PlayerInfo>, std::string> get_users();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<void, std::string> register_user(
                const RegisterRequest& register_request
        );
    };


} // namespace lobby
