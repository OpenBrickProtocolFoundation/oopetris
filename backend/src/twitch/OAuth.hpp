#pragma once

#include <string>
#include <vector>

// https://dev.twitch.tv/docs/authentication/getting-tokens-oauth/#authorization-code-grant-flow
namespace twitch {

    struct OAuthInfo {
        std::string url;
        std::string security_string;
    };

    namespace Scope {
        namespace User {
            namespace Read {
                static std::string Subscription = "user:read:subscriptions";
                static std::string Email = "user:read:email";
            }; // namespace Read
        };     // namespace User
        //TODO add more
    }; // namespace Scope


    OAuthInfo getAuthenticationURL(std::vector<std::string> scopes);

    std::string scopesToEscapedString(std::vector<std::string> scopes);

    std::string escapeString(std::string string);

}; // namespace twitch
