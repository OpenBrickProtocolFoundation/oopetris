
#pragma once

#include <string>


#include <core/helper/parse_json.hpp>

namespace lobby {

    struct ErrorResponse {
        std::string message;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorResponse, message)

    struct VersionResult {
        std::string version;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VersionResult, version)

    struct PlayerInfo {
        std::string id;
        std::string name;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerInfo, id, name)

    struct LobbyInfo {
        std::string id;
        std::string name;
        int size;
        int num_players_in_lobby;
        PlayerInfo host_info;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LobbyInfo, id, name, size, num_players_in_lobby, host_info)

    struct LobbyDetail {
        std::string name;
        int size;
        PlayerInfo host_info;
        std::vector<PlayerInfo> player_infos;
        std::optional<int> gameserver_port;
    };


    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LobbyDetail, name, size, host_info, player_infos, gameserver_port)

    struct LobbyCreateResponse {
        std::string id;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LobbyCreateResponse, id)

    struct CreateLobbyRequest {
        std::string name;
        int size;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateLobbyRequest, name, size)

    struct Credentials {
        std::string username;
        std::string password;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Credentials, username, password)

    struct LoginResponse {
        std::string jwt;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LoginResponse, jwt)

    struct RegisterRequest {
        std::string username;
        std::string password;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RegisterRequest, username, password)

} // namespace lobby
