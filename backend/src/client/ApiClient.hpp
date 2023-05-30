
#pragma once

#include <oatpp/core/data/mapping/type/Object.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/web/client/ApiClient.hpp>

#include OATPP_CODEGEN_BEGIN(ApiClient)

class ApiClient : public oatpp::web::client::ApiClient {

    API_CLIENT_INIT(ApiClient)

    API_CALL("GET", "/get", apiGet)

    API_CALL_ASYNC("GET", "/get", apiGetAsync)
};

#include OATPP_CODEGEN_END(ApiClient)
