
#include "OAuth.hpp"
#include "../Helper.hpp"
#include "Utils.hpp"

twitch::OAuthInfo twitch::getAuthenticationURL(std::vector<std::string> scopes) {

    const std::string escaped_scopes = twitch::scopesToEscapedString(scopes);
    const std::string state = utils::randomAsciiString(16);

    const std::string url = std::string("https://id.twitch.tv/oauth2/authorize?client_id=") + TWITCH_CLIENT_ID
                            + "&redirect_uri=" + SERVER_URL
                            + "/twitch/callback&response_type=code&scope=" + escaped_scopes + "&state=" + state;

    return twitch::OAuthInfo{ url, state };
}


std::string twitch::scopesToEscapedString(std::vector<std::string> scopes) {
    auto result = std::stringstream{};
    for (size_t i = 0; i < scopes.size(); i++) {
        if (i != 0) {
            result << "+";
        }
        result << twitch::escapeString(scopes[i]);
    }


    return result.str();
}

std::string twitch::escapeString(std::string input) {
    auto result = input;
    //TODO make this better, and add more escape chars
    auto replacements = utils::ReplaceFields{};
    replacements.emplace_back(":", "%3A");


    for (const auto& [in, out] : replacements) {
        utils::replaceStringInPlace(result, in, out);
    }

    return result;
}
