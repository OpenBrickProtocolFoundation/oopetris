

#pragma once


#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(_MSC_VER)
#pragma warning(disable : 4100)
#endif

#define CPPHTTPLIB_USE_POLL // NOLINT(cppcoreguidelines-macro-usage)

#include <httplib.h>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(default : 4100)
#endif

#include <fmt/format.h>
#include <optional>
#include <spdlog/spdlog.h>

#include "helper/windows.hpp"
#include "lobby/types.hpp"
#include <core/helper/expected.hpp>
#include <core/helper/static_string.hpp>

namespace constants {
    const constexpr auto json_content_type = "application/json";
}

namespace lobby {


    struct Client {
    private:
        httplib::Client m_client;
        std::optional<std::string> m_authentication_token;

        // lobby commit used: https://github.com/OpenBrickProtocolFoundation/lobby/commit/2e0c8d05592f4e4d08437e6cb754a30f02c4e97c
        static constexpr StaticString supported_version{ "0.1.0" };

        [[nodiscard]] helper::expected<void, std::string> check_compatibility();

        [[nodiscard]] helper::expected<void, std::string> check_reachability();

        explicit Client(const std::string& api_url);

        helper::expected<lobby::VersionResult, std::string> get_version();

        helper::expected<lobby::LoginResponse, std::string> login(const lobby::Credentials& credentials);


    public:
        OOPETRIS_GRAPHICS_EXPORTED Client(Client&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED Client& operator=(Client&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED Client(const Client& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED Client& operator=(const Client& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~Client();

        OOPETRIS_GRAPHICS_EXPORTED
                [[nodiscard]] helper::expected<Client, std::string> static get_client(const std::string& url);


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
