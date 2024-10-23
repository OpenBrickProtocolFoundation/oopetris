
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

#include "./client.hpp"

namespace oopetris::http::implementation {

    struct ActualResult : ::oopetris::http::Result {
    private:
        httplib::Result m_result;

    public:
        OOPETRIS_GRAPHICS_EXPORTED ActualResult(httplib::Result&& result);

        OOPETRIS_GRAPHICS_EXPORTED ~ActualResult() override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string> get_header(const std::string& key
        ) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string body() const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] int status() const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string> get_error() const override;
    };


    struct ActualClient : ::oopetris::http::Client {
    private:
        httplib::Client m_client;

    public:
        OOPETRIS_GRAPHICS_EXPORTED ActualClient(ActualClient&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(ActualClient&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualClient(const ActualClient& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(const ActualClient& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~ActualClient() override;

        OOPETRIS_GRAPHICS_EXPORTED explicit ActualClient(const std::string& api_url);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result> Get(const std::string& url) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result> Delete(const std::string& url) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result> Post(
                const std::string& url,
                const std::optional<std::pair<std::string, std::string>>& payload = std::nullopt
        ) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result> Put(
                const std::string& url,
                const std::optional<std::pair<std::string, std::string>>& payload = std::nullopt
        ) override;

        OOPETRIS_GRAPHICS_EXPORTED void SetBearerAuth(const std::string& token) override;
    };


} // namespace oopetris::http::implementation
