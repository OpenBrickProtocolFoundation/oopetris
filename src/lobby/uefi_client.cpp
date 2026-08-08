
#include "./uefi_client.hpp"

#include <core/helper/magic_enum_wrapper.hpp>
#include <core/helper/utils.hpp>

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/ManagedNetwork.h>

#include <vector>

#define TRANSFORM_RESULT(result) \
    std::make_unique<ActualResult>((result).first, (result).second) //NOLINT(cppcoreguidelines-macro-usage

namespace {

    using HttpHeaders = oopetris::http::implementation::details::HttpHeaders;

    [[nodiscard]] int map_status_code_enum(EFI_HTTP_STATUS_CODE code) {
        switch (code) {
            case HTTP_STATUS_UNSUPPORTED_STATUS:
                return -1;
            case HTTP_STATUS_100_CONTINUE:
                return 100;
            case HTTP_STATUS_101_SWITCHING_PROTOCOLS:
                return 101;
            case HTTP_STATUS_200_OK:
                return 200;
            case HTTP_STATUS_201_CREATED:
                return 201;
            case HTTP_STATUS_202_ACCEPTED:
                return 202;
            case HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION:
                return 203;
            case HTTP_STATUS_204_NO_CONTENT:
                return 204;
            case HTTP_STATUS_205_RESET_CONTENT:
                return 205;
            case HTTP_STATUS_206_PARTIAL_CONTENT:
                return 206;
            case HTTP_STATUS_300_MULTIPLE_CHOICES:
                return 300;
            case HTTP_STATUS_301_MOVED_PERMANENTLY:
                return 301;
            case HTTP_STATUS_302_FOUND:
                return 302;
            case HTTP_STATUS_303_SEE_OTHER:
                return 303;
            case HTTP_STATUS_304_NOT_MODIFIED:
                return 304;
            case HTTP_STATUS_305_USE_PROXY:
                return 305;
            case HTTP_STATUS_307_TEMPORARY_REDIRECT:
                return 307;
            case HTTP_STATUS_400_BAD_REQUEST:
                return 400;
            case HTTP_STATUS_401_UNAUTHORIZED:
                return 401;
            case HTTP_STATUS_402_PAYMENT_REQUIRED:
                return 402;
            case HTTP_STATUS_403_FORBIDDEN:
                return 403;
            case HTTP_STATUS_404_NOT_FOUND:
                return 404;
            case HTTP_STATUS_405_METHOD_NOT_ALLOWED:
                return 405;
            case HTTP_STATUS_406_NOT_ACCEPTABLE:
                return 406;
            case HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED:
                return 407;
            case HTTP_STATUS_408_REQUEST_TIME_OUT:
                return 408;
            case HTTP_STATUS_409_CONFLICT:
                return 409;
            case HTTP_STATUS_410_GONE:
                return 410;
            case HTTP_STATUS_411_LENGTH_REQUIRED:
                return 411;
            case HTTP_STATUS_412_PRECONDITION_FAILED:
                return 412;
            case HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE:
                return 413;
            case HTTP_STATUS_414_REQUEST_URI_TOO_LARGE:
                return 414;
            case HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE:
                return 415;
            case HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIED:
                return 416;
            case HTTP_STATUS_417_EXPECTATION_FAILED:
                return 417;
            case HTTP_STATUS_500_INTERNAL_SERVER_ERROR:
                return 500;
            case HTTP_STATUS_501_NOT_IMPLEMENTED:
                return 501;
            case HTTP_STATUS_502_BAD_GATEWAY:
                return 502;
            case HTTP_STATUS_503_SERVICE_UNAVAILABLE:
                return 503;
            case HTTP_STATUS_504_GATEWAY_TIME_OUT:
                return 504;
            case HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED:
                return 505;
            case HTTP_STATUS_308_PERMANENT_REDIRECT:
                return 308;
            case HTTP_STATUS_429_TOO_MANY_REQUESTS:
                return 429;
            default:
                return -1;
        }
    }

    [[nodiscard]] std::string map_efi_status_to_string(EFI_STATUS status) {

        switch (status) {
            case EFI_SUCCESS:
                return "SUCCESS";
            case EFI_LOAD_ERROR:
                return "LOAD_ERROR";
            case EFI_INVALID_PARAMETER:
                return "INVALID_PARAMETER";
            case EFI_UNSUPPORTED:
                return "UNSUPPORTED";
            case EFI_BAD_BUFFER_SIZE:
                return "BAD_BUFFER_SIZE";
            case EFI_BUFFER_TOO_SMALL:
                return "BUFFER_TOO_SMALL";
            case EFI_NOT_READY:
                return "NOT_READY";
            case EFI_DEVICE_ERROR:
                return "DEVICE_ERROR";
            case EFI_WRITE_PROTECTED:
                return "WRITE_PROTECTED";
            case EFI_OUT_OF_RESOURCES:
                return "OUT_OF_RESOURCES";
            case EFI_VOLUME_CORRUPTED:
                return "VOLUME_CORRUPTED";
            case EFI_VOLUME_FULL:
                return "VOLUME_FULL";
            case EFI_NO_MEDIA:
                return "NO_MEDIA";
            case EFI_MEDIA_CHANGED:
                return "MEDIA_CHANGED";
            case EFI_NOT_FOUND:
                return "NOT_FOUND";
            case EFI_ACCESS_DENIED:
                return "ACCESS_DENIED";
            case EFI_NO_RESPONSE:
                return "NO_RESPONSE";
            case EFI_NO_MAPPING:
                return "NO_MAPPING";
            case EFI_TIMEOUT:
                return "TIMEOUT";
            case EFI_NOT_STARTED:
                return "NOT_STARTED";
            case EFI_ALREADY_STARTED:
                return "ALREADY_STARTED";
            case EFI_ABORTED:
                return "ABORTED";
            case EFI_ICMP_ERROR:
                return "ICMP_ERROR";
            case EFI_TFTP_ERROR:
                return "TFTP_ERROR";
            case EFI_PROTOCOL_ERROR:
                return "PROTOCOL_ERROR";
            case EFI_INCOMPATIBLE_VERSION:
                return "INCOMPATIBLE_VERSION";
            case EFI_SECURITY_VIOLATION:
                return "SECURITY_VIOLATION";
            case EFI_CRC_ERROR:
                return "CRC_ERROR";
            case EFI_END_OF_MEDIA:
                return "END_OF_MEDIA";
            case EFI_END_OF_FILE:
                return "END_OF_FILE";
            case EFI_INVALID_LANGUAGE:
                return "INVALID_LANGUAGE";
            case EFI_COMPROMISED_DATA:
                return "COMPROMISED_DATA";
            case EFI_IP_ADDRESS_CONFLICT:
                return "IP_ADDRESS_CONFLICT";
            case EFI_HTTP_ERROR:
                return "HTTP_ERROR";
            case EFI_WARN_UNKNOWN_GLYPH:
                return "WARN_UNKNOWN_GLYPH";
            case EFI_WARN_DELETE_FAILURE:
                return "WARN_DELETE_FAILURE";
            case EFI_WARN_WRITE_FAILURE:
                return "WARN_WRITE_FAILURE";
            case EFI_WARN_BUFFER_TOO_SMALL:
                return "WARN_BUFFER_TOO_SMALL";
            case EFI_WARN_STALE_DATA:
                return "WARN_STALE_DATA";
            case EFI_WARN_FILE_SYSTEM:
                return "WARN_FILE_SYSTEM";
            case EFI_WARN_RESET_REQUIRED:
                return "WARN_RESET_REQUIRED";
            default:
                return "<Unknown State>";
        }
    }

} // namespace

oopetris::http::implementation::ActualResult::ActualResult(
        HTTP_IO_RESPONSE_DATA&& response,
        std::unique_ptr<HTTP_IO>&& io
)
    : m_response{ std::move(response) },
      m_io{ std::move(io) } { }


oopetris::http::implementation::ActualResult::~ActualResult() {
    if (m_io == nullptr) {
        HttpIoDestroyIo(m_io.get());
        m_io = nullptr;
    }
};


oopetris::http::implementation::ActualResult::ActualResult(ActualResult&& other) noexcept
    : m_response{ std::move(other.m_response) },
      m_io{ std::move(other.m_io) } {
    other.m_io = nullptr;
}

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_header(
        const std::string& key
) const {
    for (size_t i = 0; i < m_response.HeaderCount; ++i) {
        const EFI_HTTP_HEADER& header = m_response.Headers[i];
        if (header.FieldName == key) {
            return std::string{ header.FieldValue };
        }
    }

    return std::nullopt;
}

[[nodiscard]] std::string oopetris::http::implementation::ActualResult::body() const {

    if (m_response.Body == nullptr) {
        return "";
    }


    std::string result{ m_response.Body, m_response.Body + m_response.BodyLength };

    return result;
}

[[nodiscard]] int oopetris::http::implementation::ActualResult::status() const {
    return map_status_code_enum(m_response.Response.StatusCode);
}

[[nodiscard]] std::optional<std::string> oopetris::http::implementation::ActualResult::get_error() const {


    if (m_response.Status != EFI_SUCCESS) {
        return fmt::format("Invalid EFI status: {}", map_efi_status_to_string(m_response.Status));
    }

    return std::nullopt;
}

namespace {
    using HttpData = std::pair<std::string, std::string>;
    using RequestResponse = std::pair<HTTP_IO_RESPONSE_DATA, std::unique_ptr<HTTP_IO>>;

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

    CHAR16* ToChar16(const std::string& Str) {
        CHAR16* Result = (CHAR16*) AllocateZeroPool((Str.size() + 1) * sizeof(CHAR16));

        if (Result == nullptr) {
            return nullptr;
        }

        for (size_t i = 0; i < Str.size(); ++i) {
            Result[i] = (CHAR16) (UINT8) Str[i];
        }

        return Result;
    }

    RequestResponse make_request_impl(
            EFI_HTTP_METHOD method,
            const std::string& url,
            const HttpHeaders& header,
            const std::optional<HttpData>& data,
            EFI_HANDLE image_handle,
            EFI_HANDLE controller_handle
    ) {


        std::unique_ptr<HTTP_IO> HttpIo = std::make_unique<HTTP_IO>();
        HTTP_IO_CONFIG_DATA ConfigData;
        EFI_HTTP_REQUEST_DATA Request;

        HTTP_IO_RESPONSE_DATA Response;

        ZeroMem(HttpIo.get(), sizeof(HttpIo));
        ZeroMem(&ConfigData, sizeof(ConfigData));
        ZeroMem(&Request, sizeof(Request));
        ZeroMem(&Response, sizeof(Response));


        ConfigData.Config4.HttpVersion = HttpVersion11;
        ConfigData.Config4.RequestTimeOut = 5000;
        ConfigData.Config4.ResponseTimeOut = 5000;
        ConfigData.Config4.UseDefaultAddress = TRUE;
        ConfigData.Config4.LocalPort = 0;

        EFI_STATUS Status =
                HttpIoCreateIo(image_handle, controller_handle, IP_VERSION_4, &ConfigData, NULL, NULL, HttpIo.get());
        if (EFI_ERROR(Status)) {
            Response.Status = Status;
            return RequestResponse{ Response, std::move(HttpIo) };
        }

        Request.Method = method;
        Request.Url = ToChar16(url.c_str());

        if (Request.Url == nullptr) {
            Response.Status = EFI_OUT_OF_RESOURCES;
            return RequestResponse{ Response, std::move(HttpIo) };
        }

        std::vector<EFI_HTTP_HEADER> raw_headers{};
        for (const auto& [key, value] : header) {
            raw_headers.push_back(EFI_HTTP_HEADER{ strdup(key.c_str()), strdup(value.c_str()) });
        }


        UINTN BodyLength = 0;
        VOID* Body = NULL;

        if (data.has_value()) {
            const auto& [mime_type, raw_data] = data.value();

            Body = const_cast<void*>(raw_data.data());
            BodyLength = raw_data.size();
            raw_headers.push_back(EFI_HTTP_HEADER{ strdup("Content-Type"), strdup(mime_type.c_str()) });
        }

        UINTN HeaderCount = raw_headers.size();
        EFI_HTTP_HEADER* Headers = raw_headers.data();

        Status = HttpIoSendRequest(HttpIo.get(), &Request, HeaderCount, Headers, BodyLength, Body);
        if (EFI_ERROR(Status)) {
            Response.Status = Status;
            return RequestResponse{ Response, std::move(HttpIo) };
        }

        //TODO: I am not sure who allocates the body here? do in eed to set bodylength and allocate body and make this call again with FALSE and new space?

        Status = HttpIoRecvResponse(HttpIo.get(), TRUE, &Response);
        if (EFI_ERROR(Status)) {
            Response.Status = Status;
            return RequestResponse{ Response, std::move(HttpIo) };
        }

        for (const auto& raw_header : raw_headers) {
            free(raw_header.FieldName);
            free(raw_header.FieldValue);
        }

        FreePool(Request.Url);


        return RequestResponse{ Response, std::move(HttpIo) };
    }

    RequestResponse make_request(
            EFI_HTTP_METHOD method,
            const std::string& url,
            const HttpHeaders& header,
            EFI_HANDLE image_handle,
            EFI_HANDLE controller_handle
    ) {
        return make_request_impl(method, url, header, std::nullopt, image_handle, controller_handle);
    }

    RequestResponse make_request_with_data(
            EFI_HTTP_METHOD method,
            const std::string& url,
            const HttpHeaders& header,
            const HttpData& data,
            EFI_HANDLE image_handle,
            EFI_HANDLE controller_handle
    ) {
        return make_request_impl(method, url, header, data, image_handle, controller_handle);
    }
} // namespace


oopetris::http::implementation::ActualClient::ActualClient(ActualClient&& other) noexcept
    : m_base_url{ std::move(other.m_base_url) },
      m_headers{ std::move(other.m_headers) },
      m_image_handle{ std::move(other.m_image_handle) },
      m_controller_handle{ std::move(other.m_controller_handle) } { }

oopetris::http::implementation::ActualClient::~ActualClient() = default;

oopetris::http::implementation::ActualClient::ActualClient(
        const std::string& api_url,
        EFI_HANDLE image_handle,
        EFI_HANDLE controller_handle
)
    : m_base_url{ normalize_url(api_url) },
      m_headers{},
      m_image_handle{ image_handle },
      m_controller_handle{ controller_handle } {

    //NOTE: no Accept header or compression is set here, as uefi http request doesn't support gzip (at least I think it doesn#t support it)
}

static EFI_STATUS FindNetworkController(OUT EFI_HANDLE* Controller) {
    EFI_HANDLE* Handles = NULL;
    UINTN HandleCount = 0;

    EFI_STATUS Status = gBS->LocateHandleBuffer(
            ByProtocol, &gEfiManagedNetworkServiceBindingProtocolGuid, NULL, &HandleCount, &Handles
    );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    if (HandleCount == 0) {
        FreePool(Handles);
        return EFI_NOT_FOUND;
    }

    //
    // For now, just use the first network controller.
    //
    *Controller = Handles[0];

    FreePool(Handles);

    return EFI_SUCCESS;
}

OOPETRIS_GRAPHICS_EXPORTED std::unique_ptr<oopetris::http::implementation::ActualClient>
oopetris::http::implementation::ActualClient::construct(const std::string& api_url) {
    EFI_HANDLE controller_handle = nullptr;
    EFI_STATUS status = FindNetworkController(&controller_handle);

    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{
                        fmt::format("couldn't get network controller: {}", map_efi_status_to_string(status)) }
        );
    }

    return std::make_unique<oopetris::http::implementation::ActualClient>(api_url, gImageHandle, controller_handle);
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Get(
        const std::string& url
) {

    const auto final_url = concat_url(m_base_url, url);

    return TRANSFORM_RESULT(make_request(HttpMethodGet, final_url, m_headers, m_image_handle, m_controller_handle));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Delete(
        const std::string& url
) {
    const auto final_url = concat_url(m_base_url, url);

    return TRANSFORM_RESULT(make_request(HttpMethodDelete, final_url, m_headers, m_image_handle, m_controller_handle));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Post(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {
    const auto final_url = concat_url(m_base_url, url);

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(
                make_request(HttpMethodPost, final_url, m_headers, m_image_handle, m_controller_handle)
        );
    }

    return TRANSFORM_RESULT(make_request_with_data(
            HttpMethodPost, final_url, m_headers, payload.value(), m_image_handle, m_controller_handle
    ));
}

[[nodiscard]] std::unique_ptr<oopetris::http::Result> oopetris::http::implementation::ActualClient::Put(
        const std::string& url,
        const std::optional<std::pair<std::string, std::string>>& payload
) {
    const auto final_url = concat_url(m_base_url, url);

    if (not payload.has_value()) {
        return TRANSFORM_RESULT(make_request(HttpMethodPut, final_url, m_headers, m_image_handle, m_controller_handle));
    }


    return TRANSFORM_RESULT(make_request_with_data(
            HttpMethodPut, final_url, m_headers, payload.value(), m_image_handle, m_controller_handle
    ));
}

void oopetris::http::implementation::ActualClient::SetBearerAuth(const std::string& token) {

    m_headers.at("Authorization") = fmt::format("Bearer {}", token);
}

void oopetris::http::implementation::ActualClient::ResetBearerAuth() {

    m_headers.erase("Authorization");
}
