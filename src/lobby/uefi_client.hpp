
#pragma once


#if defined(__GNUC__)
#pragma GCC diagnostic push
//TODO
//#pragma GCC diagnostic ignored "-fpermissive"

#endif

extern "C" {
#include <Uefi.h>

#include <Library/HttpIoLib.h>
}

//this i needed to mix stdlib and openssl / crypto / tls libraries

#undef NTOHL
#undef HTONL
#undef NTOHS
#undef HTONS

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


#include "./client.hpp"

#include <unordered_map>
#include <utility>

namespace oopetris::http::implementation {
    namespace details {
        using HttpHeaders = std::unordered_map<std::string, std::string>;
    } // namespace details


    struct ActualResult : ::oopetris::http::Result {
    private:
        HTTP_IO_RESPONSE_DATA m_response;
        std::unique_ptr<HTTP_IO> m_io;


    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit ActualResult(
                HTTP_IO_RESPONSE_DATA&& response,
                std::unique_ptr<HTTP_IO>&& io
        );

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
        EFI_HANDLE m_image_handle;
        EFI_HANDLE m_controller_handle;

    public:
        OOPETRIS_GRAPHICS_EXPORTED ActualClient(ActualClient&& other) noexcept;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(ActualClient&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED ActualClient(const ActualClient& other) = delete;
        OOPETRIS_GRAPHICS_EXPORTED ActualClient& operator=(const ActualClient& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED ~ActualClient() override;

        OOPETRIS_GRAPHICS_EXPORTED explicit ActualClient(
                const std::string& api_url,
                EFI_HANDLE image_handle,
                EFI_HANDLE controller_handle
        );

        OOPETRIS_GRAPHICS_EXPORTED static std::unique_ptr<oopetris::http::implementation::ActualClient> construct(
                const std::string& api_url
        );

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
