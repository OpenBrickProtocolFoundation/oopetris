
#include "ErrorHandler.hpp"

#include <iostream>

ErrorHandler::ErrorHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
    : m_objectMapper(objectMapper) { }

std::shared_ptr<ErrorHandler::OutgoingResponse>
ErrorHandler::handleError(const Status& status, const oatpp::String& message, const Headers& headers) {
    std::cout << "here in custom err ahndler\n";
    auto error = StatusDto::createShared();
    error->status = "Custom Error";
    error->code = status.code;
    error->message = message;

    auto response = ResponseFactory::createResponse(status, error, m_objectMapper);

    for (const auto& pair : headers.getAll()) {
        response->putHeader(pair.first.toString(), pair.second.toString());
    }

    return response;
}
