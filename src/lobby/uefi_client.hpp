
#pragma once

#include <Library/HttpIoLib.h>

#include "./client.hpp"

#include <vector>


namespace oopetris::http::implementation {
    namespace details {
        using HttpHeaders = std::vector<EFI_HTTP_HEADER>;
    }


    struct ActualResult : ::oopetris::http::Result {
    private:
        HTTP_IO_RESPONSE_DATA m_response;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ActualResult(HTTP_IO_RESPONSE_DATA&& response);

        OOPETRIS_GRAPHICS_EXPORTED ~ActualResult() override;

        OOPETRIS_GRAPHICS_EXPORTED ActualResult(ActualResult&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualResult& operator=(ActualResult&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualResult(const ActualResult& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualResult& operator=(const ActualResult& other) = delete;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string> get_header(
                const std::string& key
        ) const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::string body() const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED int status() const override;

        [[nodiscard]] OOPETRIS_GRAPHICS_EXPORTED std::optional<std::string> get_error() const override;
    };


    struct ActualClient : ::oopetris::http::Client {

    private:
        std::string m_base_url;
        details::HttpHeaders m_headers;

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
