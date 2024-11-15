
#pragma once

#include <emscripten/fetch.h>

#include <unordered_map>

#include "./client.hpp"


namespace oopetris::http::implementation {

    namespace details {
        using FetchHeader = std::unordered_map<std::string, std::string>;
    }

    struct ActualResult : ::oopetris::http::Result {
    private:
        std::unique_ptr<emscripten_fetch_t> m_request;
        details::FetchHeader m_response_headers;

    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ActualResult(std::unique_ptr<emscripten_fetch_t>&& request);

        OOPETRIS_GRAPHICS_EXPORTED ~ActualResult() override;

        OOPETRIS_GRAPHICS_EXPORTED ActualResult(ActualResult&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualResult& operator=(ActualResult&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualResult(const ActualResult& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualResult& operator=(const ActualResult& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string> get_header(const std::string& key
        ) const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::string body() const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] int status() const override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string> get_error() const override;
    };


    struct ActualClient : ::oopetris::http::Client {

    private:
        std::string m_base_url;
        details::FetchHeader m_headers;

    public:
        OOPETRIS_GRAPHICS_EXPORTED ActualClient(ActualClient&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(ActualClient&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualClient(const ActualClient& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(const ActualClient& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~ActualClient() override;

        OOPETRIS_GRAPHICS_EXPORTED explicit ActualClient(const std::string& api_url);

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result> Get(const std::string& url) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result> Delete(const std::string& url) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result>
        Post(const std::string& url, const std::optional<std::pair<std::string, std::string>>& payload) override;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::unique_ptr<Result>
        Put(const std::string& url, const std::optional<std::pair<std::string, std::string>>& payload) override;

        OOPETRIS_GRAPHICS_EXPORTED void SetBearerAuth(const std::string& token) override;

        OOPETRIS_GRAPHICS_EXPORTED void ResetBearerAuth() override;
    };


} // namespace oopetris::http::implementation
