#pragma once


#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/web/server/api/ApiController.hpp>

#include "../Helper.hpp"
#include "../dto/DTOs.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class StaticController : public oatpp::web::server::api::ApiController {
protected:
    typedef StaticController __ControllerType;

protected:
    StaticController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper) { }

public:
    static std::shared_ptr<StaticController> createShared(OATPP_COMPONENT(
            std::shared_ptr<ObjectMapper>,
            objectMapper
    ) // Inject objectMapper component here as default parameter
    ) {
        return std::shared_ptr<StaticController>(new StaticController(objectMapper));
    }

    ENDPOINT("GET", "*", files, REQUEST(std::shared_ptr<IncomingRequest>, request)) {
        auto filePath = request->getPathTail();
        OATPP_ASSERT_HTTP(filePath, Status::CODE_400, "Empty filename");
        if (filePath.getValue("") == "/" || filePath.getValue("") == "") {
            filePath = oatpp::String("/index.html");
        }
        auto buffer = utils::loadFileFromRoot(filePath);
        if (!buffer.has_value() || buffer.value().empty()) {
            return createResponse(Status::CODE_404, utils::errorTemplate(Status::CODE_404));
        }
        return createResponse(Status::CODE_200, buffer.value());
    }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen
