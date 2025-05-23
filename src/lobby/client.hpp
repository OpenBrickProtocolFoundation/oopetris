
#pragma once


#include "./constants.hpp"
#include "./types.hpp"
#include "helper/export_symbols.hpp"

#include <fmt/format.h>
#include <optional>
#include <string>
#include <utility>


#include <core/helper/expected.hpp>


namespace oopetris::http {


    struct Result { //NOLINT(cppcoreguidelines-special-member-functions)
        OOPETRIS_GRAPHICS_EXPORTED virtual ~Result();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::optional<std::string> get_header(const std::string& key
        ) const = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::string body() const = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual int status() const = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::optional<std::string> get_error() const = 0;
    };

    struct Client { //NOLINT(cppcoreguidelines-special-member-functions)
        OOPETRIS_GRAPHICS_EXPORTED virtual ~Client();

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::unique_ptr<Result>
        Get( //NOLINT(readability-identifier-naming)
                const std::string& url
        ) = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::unique_ptr<Result>
        Delete( //NOLINT(readability-identifier-naming)
                const std::string& url
        ) = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::unique_ptr<Result>
        Post( //NOLINT(readability-identifier-naming)
                const std::string& url,
                const std::optional<std::pair<std::string, std::string>>& payload
        ) = 0;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED virtual std::unique_ptr<Result>
        Put( //NOLINT(readability-identifier-naming)
                const std::string& url,
                const std::optional<std::pair<std::string, std::string>>& payload
        ) = 0;

        OOPETRIS_GRAPHICS_EXPORTED virtual void SetBearerAuth( //NOLINT(readability-identifier-naming)
                const std::string& token
        ) = 0;

        OOPETRIS_GRAPHICS_EXPORTED virtual void ResetBearerAuth() = 0; //NOLINT(readability-identifier-naming)
    };

    [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string status_message(int status);

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
