
#pragma once


#include "./constants.hpp"
#include "./types.hpp"
#include "helper/windows.hpp"

#include <fmt/format.h>
#include <optional>
#include <string>
#include <utility>


#include <core/helper/expected.hpp>


namespace oopetris::http {

    struct Result {
        OOPETRIS_GRAPHICS_EXPORTED virtual ~Result();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::optional<std::string> get_header(const std::string& key
        ) const = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::string body() const = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual int status() const = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::optional<std::string> get_error() const = 0;
    };

    struct Client {
        OOPETRIS_GRAPHICS_EXPORTED virtual ~Client();

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::unique_ptr<Result> Get(const std::string& url) = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::unique_ptr<Result> Delete(const std::string& url) = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::unique_ptr<Result> Post(
                const std::string& url,
                const std::optional<std::pair<std::string, std::string>>& payload = std::nullopt
        ) = 0;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] virtual std::unique_ptr<Result> Put(
                const std::string& url,
                const std::optional<std::pair<std::string, std::string>>& payload = std::nullopt
        ) = 0;

        OOPETRIS_GRAPHICS_EXPORTED virtual void SetBearerAuth(const std::string& token) = 0;
    };


    OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string status_message(int status);

    OOPETRIS_GRAPHICS_EXPORTED helper::expected<std::string, std::string> is_json_response(
            const std::unique_ptr<oopetris::http::Result>& result
    );


    OOPETRIS_GRAPHICS_EXPORTED helper::expected<lobby::ErrorResponse, std::string> is_error_message_response(
            const std::unique_ptr<oopetris::http::Result>& result
    );

    OOPETRIS_GRAPHICS_EXPORTED helper::expected<void, std::string>
    is_request_ok(const std::unique_ptr<oopetris::http::Result>& result, int ok_code = 200);

    template<typename T>
    helper::expected<T, std::string>
    get_json_from_request(const std::unique_ptr<oopetris::http::Result>& result, int ok_code = 200) {

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


} // namespace oopetris::http
