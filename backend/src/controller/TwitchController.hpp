#pragma once


#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/web/server/api/ApiController.hpp>

#include "../twitch/OAuth.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class TwitchController : public oatpp::web::server::api::ApiController {
protected:
    typedef TwitchController __ControllerType;

protected:
    TwitchController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper) { }

public:
    static std::shared_ptr<TwitchController> createShared(OATPP_COMPONENT(
            std::shared_ptr<ObjectMapper>,
            objectMapper
    ) // Inject objectMapper component here as default parameter
    ) {
        return std::shared_ptr<TwitchController>(new TwitchController(objectMapper));
    }

    // https://dev.twitch.tv/docs/authentication/getting-tokens-oauth/#authorization-code-grant-flow
    ENDPOINT("GET", "twitch/callback", files, REQUEST(std::shared_ptr<IncomingRequest>, request)) {

        const oatpp::web::protocol::http::QueryParams& params = request->getQueryParameters();

        //TODO

    }
};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen
