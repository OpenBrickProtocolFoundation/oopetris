#pragma once


#include <oatpp/core/macro/component.hpp>
#include <oatpp/web/protocol/http/Http.hpp>


#include "../db/UserDb.hpp"
#include "../dto/PageDto.hpp"
#include "../dto/StatusDto.hpp"

class UserService {
private:
    typedef oatpp::web::protocol::http::Status Status;

private:
    OATPP_COMPONENT(std::shared_ptr<UserDb>, m_database); // Inject database component
public:
    oatpp::Object<UserDto> createUser(const oatpp::Object<UserDto>& dto);
    oatpp::Object<UserDto> updateUser(const oatpp::Object<UserDto>& dto);
    oatpp::Object<UserDto> getUserById(
            const oatpp::Int32& id,
            const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection = nullptr
    );
    oatpp::Object<PageDto<oatpp::Object<UserDto>>> getAllUsers(const oatpp::UInt32& offset, const oatpp::UInt32& limit);
    oatpp::Object<StatusDto> deleteUserById(const oatpp::Int32& id);
};
