
#include "./httplib_client.hpp"


#define TRANSFORM_RESULT(result) std::make_unique<ActualResult>((result)) //NOLINT(cppcoreguidelines-macro-usage


oopetris::http::implementation::ActualResult::ActualResult(httplib::Result&& result) : m_result{ std::move(result) } { }

oopetris::http::implementation::ActualResult::~ActualResult() = default;


oopetris::http::implementation::ActualResult::ActualResult(ActualResult&& other) noexcept
    : m_result{ std::move(other.m_result) } { }

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_header(const std::string& key
) const {
    if (m_result->has_header(key)) {
        return std::nullopt;
    }

    return m_result->get_header_value(key);
}

[[nodiscard]] std::string oopetris::http::implementation::ActualResult::body() const {
    return m_result->body;
}

[[nodiscard]] int oopetris::http::implementation::ActualResult::status() const {
    return m_result->status;
}

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_error() const {

    if (not m_result) {
        return httplib::to_string(m_result.error());
    }

    return std::nullopt;
}


oopetris::http::implementation::ActualClient::ActualClient(ActualClient&& other) noexcept
    : m_client{ std::move(other.m_client) } { }

oopetris::http::implementation::ActualClient::~ActualClient() = default;

oopetris::http::implementation::ActualClient::ActualClient(const std::string& api_url) : m_client{ api_url } {
    // clang-format off
            m_client.set_default_headers({
#if defined(CPPHTTPLIB_ZLIB_SUPPORT) || defined(CPPHTTPLIB_BROTLI_SUPPORT)
                    { "Accept-Encoding",

#if defined(CPPHTTPLIB_ZLIB_SUPPORT)
                        "gzip, deflate"
#endif
#if defined(CPPHTTPLIB_ZLIB_SUPPORT) && defined(CPPHTTPLIB_BROTLI_SUPPORT)
                        ", "
#endif
#if defined(CPPHTTPLIB_BROTLI_SUPPORT)
                        "br"
#endif
                        },
#endif
            // clang-format on
            { "Accept", ::http::constants::json_content_type } });

#if defined(CPPHTTPLIB_ZLIB_SUPPORT) || defined(CPPHTTPLIB_BROTLI_SUPPORT)
    m_client.set_compress(true);
    m_client.set_decompress(true);
#endif
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Get(
        const std::string& url
) {
    return TRANSFORM_RESULT(m_client.Get(url));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Delete(
        const std::string& url
) {
    return TRANSFORM_RESULT(m_client.Delete(url));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Post(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(m_client.Post(url));
    }

    auto [content, content_type] = payload.value();

    return TRANSFORM_RESULT(m_client.Post(url, content, content_type));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Put(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(m_client.Put(url));
    }

    auto [content, content_type] = payload.value();

    return TRANSFORM_RESULT(m_client.Put(url, content, content_type));
}

void oopetris::http::implementation::ActualClient::SetBearerAuth(const std::string& token) {

    m_client.set_bearer_token_auth(token);
}

void oopetris::http::implementation::ActualClient::ResetBearerAuth() {

    m_client.set_bearer_token_auth("");
}
