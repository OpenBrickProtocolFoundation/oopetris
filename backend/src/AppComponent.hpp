
#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "client/MyApiClient.hpp"

#include "oatpp-openssl/Config.hpp"
#include "oatpp-openssl/client/ConnectionProvider.hpp"
#include "oatpp-openssl/server/ConnectionProvider.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/network/tcp/client/ConnectionProvider.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/component.hpp"

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:
    /**
   *  Create ConnectionProvider component which listens on the port
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
    ([] {
        OATPP_LOGD("oatpp::openssl::Config", "pem='%s'", CERT_PEM_PATH);
        OATPP_LOGD("oatpp::openssl::Config", "crt='%s'", CERT_CRT_PATH);
        auto config =
                oatpp::openssl::Config::createDefaultServerConfigShared(CERT_CRT_PATH, CERT_PEM_PATH /* private key */);

        /**
     * if you see such error:
     * oatpp::openssl::server::ConnectionProvider:Error on call to 'tls_configure'. ssl context failure
     * It might be because you have several ssl libraries installed on your machine.
     * Try to make sure you are using libtls, libssl, and libcrypto from the same package
     */

        return oatpp::openssl::server::ConnectionProvider::createShared(
                config, { "0.0.0.0", 8443, oatpp::network::Address::IP_4 }
        );
    }());

    /**
   *  Create Router component
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
    ([] { return oatpp::web::server::HttpRouter::createShared(); }());

    /**
   *  Create ConnectionHandler component which uses Router component to route requests
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
    ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
        /* Async ConnectionHandler for Async IO and Coroutine based endpoints */
        return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router);
    }());

    /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)
    ([] {
        auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
        auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
        deserializerConfig->allowUnknownFields = false;
        auto objectMapper =
                oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);
        return objectMapper;
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ClientConnectionProvider>, sslClientConnectionProvider)
    ("clientConnectionProvider", [] {
        auto config = oatpp::openssl::Config::createShared();
        // tls_config_insecure_noverifycert(config->getTLSConfig());
        // tls_config_insecure_noverifyname(config->getTLSConfig());
        return oatpp::openssl::client::ConnectionProvider::createShared(config, { "httpbin.org", 443 });
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<MyApiClient>, myApiClient)
    ([] {
        OATPP_COMPONENT(
                std::shared_ptr<oatpp::network::ClientConnectionProvider>, connectionProvider,
                "clientConnectionProvider"
        );
        OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
        auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);
        return MyApiClient::createShared(requestExecutor, objectMapper);
    }());
};

#endif /* AppComponent_hpp */
