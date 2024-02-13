

#pragma once

#define CPPHTTPLIB_USE_POLL // NOLINT(cppcoreguidelines-macro-usage)

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#include <httplib.h>

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif


#include <fmt/format.h>
#include <tl/expected.hpp>

#include "helper/static_string.hpp"
#include "lobby/types.hpp"

namespace {


    template<typename T>
    tl::expected<T, std::string> get_json_from_request(const httplib::Result& result, int ok_code = 200) {

        if (not result) {
            return tl::make_unexpected(fmt::format("Request failed with: {}", httplib::to_string(result.error())));
        }

        if (result->status == 401) {
            return tl::make_unexpected("Unauthorized");
        }


        if (result->status != ok_code) {
            return tl::make_unexpected(
                    fmt::format("Status code was not {} but {}", ok_code, httplib::status_message(result->status))
            );
        }

        if (not result->has_header("Content-Type")) {
            return tl::make_unexpected("Content-Type not set!");
        }

        if (const auto value = result->get_header_value("Content-Type"); value != "application/json") {
            return tl::make_unexpected(fmt::format("Content-Type is not json but {}", value));
        }


        const auto parsed = json::try_parse_json<T>(result->body);

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


        static constexpr StaticString supported_version{ "0.1.0" };

        void check_compatibility() {
            const auto server_version = get_version();

            if (not server_version.has_value()) {
                throw std::runtime_error(fmt::format(
                        "Connecting to unsupported server, he can't report his version\nGot error: {}",
                        server_version.error()
                ));
            }

            const auto& version = server_version.value();

            //TODO: if version is semver, support semver comparison
            if (Client::supported_version.string() != version.version) {
                throw std::runtime_error(fmt::format(
                        "Connecting to unsupported server, version is {}, but we support only {}",
                        Client::supported_version.string(), version.version
                ));
            }
        }


    public:
        explicit Client(const std::string& api_url) : m_client{ api_url } {

            check_compatibility();
        }

        //TODO: wait for https://github.com/OpenBrickProtocolFoundation/lobby/pull/4
        tl::expected<VersionResult, std::string> get_version() {
            auto res = m_client.Get("/version");

            return get_json_from_request<VersionResult>(res);
        }
    };


} // namespace lobby
