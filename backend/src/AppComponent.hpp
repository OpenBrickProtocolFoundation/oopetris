#pragma once

#include <oatpp-openssl/Config.hpp>
#include <oatpp-openssl/client/ConnectionProvider.hpp>
#include <oatpp-openssl/server/ConnectionProvider.hpp>
#include <oatpp-zlib/EncoderProvider.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/network/tcp/client/ConnectionProvider.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/web/client/HttpRequestExecutor.hpp>
#include <oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp>
#include <oatpp/web/server/HttpConnectionHandler.hpp>
#include <oatpp/web/server/HttpRouter.hpp>

#include "DatabaseComponent.hpp"
#include "ErrorHandler.hpp"
#include "SwaggerComponent.hpp"

#ifndef LISTEN_PORT
#define LISTEN_PORT 8443
#endif


/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:
    /**
   *  Swagger component
   */
    SwaggerComponent swaggerComponent;

    /**
   * Database component
   */
    DatabaseComponent databaseComponent;


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
                config, { "0.0.0.0", LISTEN_PORT, oatpp::network::Address::IP_4 }
        );
    }());

    /**
   *  Create Router component
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
    ([] { return oatpp::web::server::HttpRouter::createShared(); }());

    /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API and to serialize/deserialize DTOs in Controller's API
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


    /**
   *  Create ConnectionHandler component which uses Router component to route requests
   */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
    ([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component

        OATPP_COMPONENT(
                std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper
        ); // get ObjectMapper component


        /* Create HttpProcessor::Components */
        auto components = std::make_shared<oatpp::web::server::HttpProcessor::Components>(router);

        /* Add content decoders */
        auto decoders = std::make_shared<oatpp::web::protocol::http::encoding::ProviderCollection>();

        decoders->add(std::make_shared<oatpp::zlib::DeflateDecoderProvider>());
        decoders->add(std::make_shared<oatpp::zlib::GzipDecoderProvider>());

        /* Set Body Decoder */
        components->bodyDecoder = std::make_shared<oatpp::web::protocol::http::incoming::SimpleBodyDecoder>(decoders);

        /* Add content encoders */
        auto encoders = std::make_shared<oatpp::web::protocol::http::encoding::ProviderCollection>();

        encoders->add(std::make_shared<oatpp::zlib::DeflateEncoderProvider>());
        encoders->add(std::make_shared<oatpp::zlib::GzipEncoderProvider>());

        /* Set content encoders */
        components->contentEncodingProviders = encoders;


        auto connectionHandler = std::make_shared<oatpp::web::server::HttpConnectionHandler>(components);
        connectionHandler->setErrorHandler(std::make_shared<ErrorHandler>(objectMapper));
        return connectionHandler;
    }());

};
