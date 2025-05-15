
#pragma once


#include "./client.hpp"

#if defined(__3DS__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include <cpr/cpr.h>


#if defined(__3DS__)
#pragma GCC diagnostic pop
#endif


namespace oopetris::http::implementation {

    struct ActualResult : ::oopetris::http::Result {
    private:
        cpr::Response m_result;


    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ActualResult(cpr::Response&& result);

        OOPETRIS_GRAPHICS_EXPORTED ~ActualResult() override;

        OOPETRIS_GRAPHICS_EXPORTED ActualResult(ActualResult&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualResult& operator=(ActualResult&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualResult(const ActualResult& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualResult& operator=(const ActualResult& other) = delete;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string> get_header(const std::string& key
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string body() const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED int status() const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string> get_error() const override;
    };


    struct ActualClient : ::oopetris::http::Client {
    private:
        std::unique_ptr<cpr::Session> m_session;
        std::string m_base_url;

        void set_url(const std::string& url);

    public:
        OOPETRIS_GRAPHICS_EXPORTED ActualClient(ActualClient&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(ActualClient&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualClient(const ActualClient& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(const ActualClient& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~ActualClient() override;

        OOPETRIS_GRAPHICS_EXPORTED explicit ActualClient(const std::string& api_url);

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::unique_ptr<Result> Get(const std::string& url) override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::unique_ptr<Result> Delete(const std::string& url) override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::unique_ptr<Result>
        Post(const std::string& url, const std::optional<std::pair<std::string, std::string>>& payload) override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::unique_ptr<Result>
        Put(const std::string& url, const std::optional<std::pair<std::string, std::string>>& payload) override;

        OOPETRIS_GRAPHICS_EXPORTED void SetBearerAuth(const std::string& token) override;

        OOPETRIS_GRAPHICS_EXPORTED void ResetBearerAuth() override;
    };


} // namespace oopetris::http::implementation
