
#include "./web_client.hpp"

#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>

// Note: this uses emscripten fetch
//see: https://emscripten.org/docs/api_reference/fetch.html

//Note: Synchronous Emscripten Fetch operations are subject to a number of restrictions, depending on which Emscripten build mode (linker flags) is used:
// we use: -pthread: Synchronous Fetch operations are available on pthreads, but not on the main thread.

#define TRANSFORM_RESULT(result) std::make_unique<ActualResult>((result)) //NOLINT(cppcoreguidelines-macro-usage

namespace {
    using FetchHeader = oopetris::http::implementation::details::FetchHeader;

    FetchHeader get_headers(const std::unique_ptr<emscripten_fetch_t>& request) {

        auto header_length = emscripten_fetch_get_response_headers_length(request.get());

        auto* headersRawDst = new char[header_length + 1];
        emscripten_fetch_get_response_headers(request.get(), headersRawDst, header_length + 1);

        char** unpackedHeaders = emscripten_fetch_unpack_response_headers(headersRawDst);

        FetchHeader result{};

        u32 i = 0;
        while (true) {
            auto* headerKey = unpackedHeaders[i];

            if (headerKey == nullptr) {
                break;
            }

            auto* headerValue = unpackedHeaders[i + 1];
            if (headerValue == nullptr) {
                //this is an error, since this has to be a valid char*
                //TODO: report that properly
                break;
            }

            result.at(headerKey) = std::string{ headerValue };

            i += 2;
        }

        delete[] headersRawDst;

        emscripten_fetch_free_unpacked_response_headers(unpackedHeaders);

        return result;
    }

    // Specifies the readyState of the XHR request:
    // 0: UNSENT: request not sent yet
    // 1: OPENED: emscripten_fetch has been called.
    // 2: HEADERS_RECEIVED: emscripten_fetch has been called, and headers and
    //    status are available.
    // 3: LOADING: download in progress.
    // 4: DONE: download finished.
    // See https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
    enum class FetchReadyState {
        UNSENT = 0,
        OPENED = 1,
        HEADERS_RECEIVED = 2,
        LOADING = 3,
        DONE = 4,
    };

    const constexpr u32 max_method_size = 32;

} // namespace


oopetris::http::implementation::ActualResult::ActualResult(std::unique_ptr<emscripten_fetch_t>&& request)
    : m_request{ std::move(request) },
      m_response_headers{ get_headers(m_request) } { }

oopetris::http::implementation::ActualResult::~ActualResult() {
    if (m_request) {
        emscripten_fetch_close(m_request.get());
    }
};


oopetris::http::implementation::ActualResult::ActualResult(ActualResult&& other) noexcept
    : m_request{ std::move(other.m_request) },
      m_response_headers{ std::move(other.m_response_headers) } { }

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_header(const std::string& key
) const {
    if (m_response_headers.contains(key)) {
        return std::nullopt;
    }

    return m_response_headers.at(key);
}

[[nodiscard]] std::string oopetris::http::implementation::ActualResult::body() const {

    //TODO: test if this is correct
    auto size = m_request->numBytes;

    std::string result{ m_request->data, m_request->data + size };

    return result;
}

[[nodiscard]] int oopetris::http::implementation::ActualResult::status() const {
    return m_request->status;
}

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_error() const {

    auto readyState = static_cast<FetchReadyState>(m_request->readyState);

    if (readyState != FetchReadyState::DONE) {
        return fmt::format("Invalid readyState: {}", magic_enum::enum_name(readyState));
    }

    return std::nullopt;
}

namespace {
    using FetchData = std::pair<std::string, std::string>;

    std::string normalize_url(const std::string& value) {
        if (value.ends_with("/")) {
            return value.substr(0, value.size() - 1);
        }

        return value;
    }

    std::string concat_url(const std::string& normalized_base, const std::string& value) {
        if (value.starts_with("/")) {
            return normalized_base + value;
        }

        return normalized_base + "/" + value;
    }

    std::unique_ptr<emscripten_fetch_t> make_request_impl(
            const std::string& method,
            const std::string& url,
            const FetchHeader& header,
            const std::optional<FetchData>& data
    ) {
        emscripten_fetch_attr_t attr{};
        emscripten_fetch_attr_init(&attr);

        ASSERT(method.size() <= (max_method_size - 1));
        std::memcpy(attr.requestMethod, method.c_str(), method.size() + 1);

        //TODO(Totto): once the http Implementation "Interface" support async thing, use that:
        // see: https://emscripten.org/docs/api_reference/fetch.html#synchronous-fetches

        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;


        std::vector<const char*> raw_headers{};
        for (const auto& [key, value] : header) {
            raw_headers.push_back(key.c_str());
            raw_headers.push_back(value.c_str());
        }

        raw_headers.push_back(nullptr);

        attr.requestHeaders = raw_headers.data();

        if (data.has_value()) {
            const auto& [mime_type, raw_data] = data.value();

            attr.overriddenMimeType = mime_type.c_str();
            attr.requestData = raw_data.c_str();
            attr.requestDataSize = raw_data.size();
        }

        emscripten_fetch_t* result = emscripten_fetch(&attr, url.c_str());

        return std::unique_ptr<emscripten_fetch_t>(result);
    }

    std::unique_ptr<emscripten_fetch_t>
    make_request(const std::string& method, const std::string& url, const FetchHeader& header) {
        return make_request_impl(method, url, header, std::nullopt);
    }

    std::unique_ptr<emscripten_fetch_t> make_request_with_data(
            const std::string& method,
            const std::string& url,
            const FetchHeader& header,
            const FetchData& data
    ) {
        return make_request_impl(method, url, header, data);
    }
} // namespace


oopetris::http::implementation::ActualClient::ActualClient(ActualClient&& other) noexcept
    : m_base_url{ std::move(other.m_base_url) },
      m_headers{ std::move(other.m_headers) } { }

oopetris::http::implementation::ActualClient::~ActualClient() = default;

oopetris::http::implementation::ActualClient::ActualClient(const std::string& api_url)
    : m_base_url{ normalize_url(api_url) },
      m_headers{} {

    //NOTE: no Accept header or compression is set here, as emscriptens fetch does provide reasonable defaults (depending on what the browser supports)
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Get(
        const std::string& url
) {

    const auto final_url = concat_url(m_base_url, url);

    return TRANSFORM_RESULT(make_request("GET", final_url, m_headers));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Delete(
        const std::string& url
) {
    const auto final_url = concat_url(m_base_url, url);

    return TRANSFORM_RESULT(make_request("DELETE", final_url, m_headers));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Post(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {
    const auto final_url = concat_url(m_base_url, url);

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(make_request("POST", final_url, m_headers));
    }

    auto [content, content_type] = payload.value();

    return TRANSFORM_RESULT(make_request_with_data("POST", final_url, m_headers, payload.value()));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Put(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {
    const auto final_url = concat_url(m_base_url, url);

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(make_request("PUT", final_url, m_headers));
    }

    auto [content, content_type] = payload.value();

    return TRANSFORM_RESULT(make_request_with_data("PUT", final_url, m_headers, payload.value()));
}

void oopetris::http::implementation::ActualClient::SetBearerAuth(const std::string& token) {

    m_headers.at("Authorization") = fmt::format("Bearer {}", token);
}

void oopetris::http::implementation::ActualClient::ResetBearerAuth() {

    m_headers.erase("Authorization");
}
