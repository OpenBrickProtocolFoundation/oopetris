#pragma once

#include "../dto/DTOs.hpp"
#include <filesystem>
#include <oatpp/core/data/mapping/type/Primitive.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/web/server/api/ApiController.hpp>

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

    //TODO use tl::optonal
    oatpp::String loadFileFromRoot(const oatpp::String& inputPath) {
        std::string normalizedInputPath = inputPath.getValue("index.html");
        if (normalizedInputPath.at(0) == '/') {
            normalizedInputPath.erase(0, 1);
        }
        const auto filePath = std::filesystem::path(ROOT_PATH) / normalizedInputPath;
        if (!std::filesystem::exists(filePath)) {
            return oatpp::String{ "" };
        }
        return oatpp::String::loadFromFile(filePath.c_str());
    };

    //from: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    void replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }


    oatpp::String errorTemplate(Status status) {
        const std::string errorPagePath = oatpp::String("/error/" + std::to_string(status.code) + ".html");
        std::string rawTemplate = loadFileFromRoot(errorPagePath);

        replaceStringInPlace(rawTemplate, "%{HOSTNAME}", HOSTNAME);
        return rawTemplate;
    }


    ENDPOINT("GET", "*", files, REQUEST(std::shared_ptr<IncomingRequest>, request)) {
        auto filePath = request->getPathTail();
        OATPP_ASSERT_HTTP(filePath, Status::CODE_400, "Empty filename");
        if (filePath.getValue("") == "/" or filePath.getValue("") == "") {
            filePath = oatpp::String{ "/index.html" };
        }
        oatpp::String buffer = loadFileFromRoot(filePath);
        if (buffer.getValue("").empty()) {
            return createResponse(Status::CODE_404, errorTemplate(Status::CODE_404));
        }
        return createResponse(Status::CODE_200, buffer);
    }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen
