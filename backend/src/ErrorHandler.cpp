
#include "ErrorHandler.hpp"
#include "Helper.hpp"

ErrorHandler::ErrorHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
    : m_objectMapper(objectMapper) { }

std::shared_ptr<ErrorHandler::OutgoingResponse>
ErrorHandler::handleError(const Status& status, const oatpp::String& message, const Headers& headers) {

    auto replacements = utils::ReplaceFields{};
    replacements.emplace_back("MESSAGE", message);

    oatpp::String errorContent = utils::errorTemplate(status, replacements);


    auto response = ResponseFactory::createResponse(status, errorContent, m_objectMapper);

    for (const auto& pair : headers.getAll()) {
        response->putHeader(pair.first.toString(), pair.second.toString());
    }

    return response;
}
