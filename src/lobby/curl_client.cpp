
#include "./curl_client.hpp"


#define TRANSFORM_RESULT(result) std::make_unique<ActualResult>((result)) //NOLINT(cppcoreguidelines-macro-usage)


oopetris::http::implementation::ActualResult::ActualResult(cpr::Response&& result) : m_result{ std::move(result) } { }

oopetris::http::implementation::ActualResult::~ActualResult() = default;

oopetris::http::implementation::ActualResult::ActualResult(ActualResult&& other) noexcept
    : m_result{ std::move(other.m_result) } { }

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_header(const std::string& key
) const {

    if (not m_result.header.contains(key)) {
        return std::nullopt;
    }

    return m_result.header.at(key);
}

[[nodiscard]] std::string oopetris::http::implementation::ActualResult::body() const {
    return m_result.text;
}

[[nodiscard]] int oopetris::http::implementation::ActualResult::status() const {
    return static_cast<int>(m_result.status_code);
}

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_error() const {

    if (static_cast<bool>(m_result.error) || m_result.status_code == 0) {
        return m_result.error.message;
    }

    return std::nullopt;
}


namespace {
    std::string normalize_url(const std::string& value) {
        if (value.ends_with("/")) {
            return value.substr(0, value.size() - 1);
        }

        return value;
    }
} // namespace

oopetris::http::implementation::ActualClient::ActualClient(ActualClient&& other) noexcept
    : m_session{ std::move(other.m_session) } { }

oopetris::http::implementation::ActualClient::~ActualClient() = default;


oopetris::http::implementation::ActualClient::ActualClient(const std::string& api_url)
    : m_base_url{ normalize_url(api_url) } {

    m_session->SetUrl(cpr::Url{ api_url });
    m_session->SetAcceptEncoding(cpr::AcceptEncoding{
            { cpr::AcceptEncodingMethods::deflate, cpr::AcceptEncodingMethods::gzip,
             cpr::AcceptEncodingMethods::zlib }
    });
    m_session->SetHeader(cpr::Header{
            { "Accept", ::http::constants::json_content_type }
    });
}


void oopetris::http::implementation::ActualClient::set_url(const std::string& url) {
    m_session->SetUrl(cpr::Url{ m_base_url, url });
}


[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Get(
        const std::string& url
) {
    set_url(url);
    return TRANSFORM_RESULT(m_session->Get());
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Delete(
        const std::string& url
) {
    set_url(url);
    return TRANSFORM_RESULT(m_session->Delete());
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Post(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {

    set_url(url);

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(m_session->Post());
    }

    auto [content, content_type] = payload.value();

    m_session->SetBody(cpr::Body{ content });

    return TRANSFORM_RESULT(m_session->Post());
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Put(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {
    set_url(url);

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(m_session->Put());
    }

    auto [content, content_type] = payload.value();

    m_session->SetBody(cpr::Body{ content });

    return TRANSFORM_RESULT(m_session->Put());
}

void oopetris::http::implementation::ActualClient::SetBearerAuth(const std::string& token) {


#if CPR_LIBCURL_VERSION_NUM >= 0x073D00
    m_session->SetBearer(token);
#else
    m_session->SetHeader(cpr::Header{ "Authorization", fmt::format("Bearer {}", token) });
#endif
}
