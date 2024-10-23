

#include "./client.hpp"


#if defined(_OOPETRIS_ONLINE_USE_CURL)
#include "./curl_client.hpp"


std::string oopetris::http::status_message([[maybe_unused]] int status) {
    return "Not Available";
}


#else

#include "./httplib_client.hpp"

std::string oopetris::http::status_message(int status) {
    return httplib::status_message(status);
}

#endif

oopetris::http::Result::~Result() = default;

oopetris::http::Client::~Client() = default;

helper::expected<std::string, std::string> oopetris::http::is_json_response(
        const std::unique_ptr<oopetris::http::Result>& result
) {
    const auto content_type = result->get_header("Content-Type");
    if (not content_type.has_value()) {
        return helper::unexpected<std::string>{ "Content-Type not set!" };
    }

    if (content_type.value() != ::http::constants::json_content_type) {
        return helper::unexpected<std::string>{ fmt::format("Content-Type is not json but {}", content_type.value()) };
    }

    return result->body();
}

helper::expected<lobby::ErrorResponse, std::string> oopetris::http::is_error_message_response(
        const std::unique_ptr<oopetris::http::Result>& result
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

helper::expected<void, std::string>
oopetris::http::is_request_ok(const std::unique_ptr<oopetris::http::Result>& result, int ok_code) {

    if (auto error = result->get_error(); error.has_value()) {
        return helper::unexpected<std::string>{ fmt::format("Request failed with: {}", error.value()) };
    }

    if (result->status() == 401) {

        const auto error_type = is_error_message_response(result);

        if (error_type.has_value()) {
            return helper::unexpected<std::string>{ fmt::format("Unauthorized: {}", error_type.value().message) };
        }

        return helper::unexpected<std::string>{ "Unauthorized" };
    }


    if (result->status() != ok_code) {

        const auto error_type = is_error_message_response(result);

        if (error_type.has_value()) {
            return helper::unexpected<std::string>{ fmt::format(
                    "Got error response with status code {}: '{}' and message: {}", result->status(),
                    oopetris::http::status_message(result->status()), error_type.value().message
            ) };
        }


        return helper::unexpected<std::string>{ fmt::format(
                "Got error response with status code {}: '{}' but expected {}", result->status(),
                oopetris::http::status_message(result->status()), ok_code
        ) };
    }

    return {};
};
