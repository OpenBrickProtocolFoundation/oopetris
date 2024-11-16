
#include "./secret.hpp"

#if defined(__linux__) || defined(__ANDROID__)

#include <core/helper/utils.hpp>
#include <fmt/format.h>
namespace {

    namespace constants {
        constexpr const char* key_type_user = "user";

        constexpr const char* key_name_prefix = "OOPetris_key__";
    } // namespace constants

    std::string get_key_name(const std::string& key) {
        return constants::key_name_prefix + key;
    }

    i64 get_id_from_name(key_serial_t keyring_id, const std::string& key) {

        const std::string full_key = get_key_name(key);

        // 0 stands for: do not create a link to another keyring
        return keyctl_search(keyring_id, constants::key_type_user, full_key.c_str(), 0);
    }


} // namespace


secret::SecretStorage::SecretStorage(KeyringType type) : m_type{ type } {

    key_serial_t key_type{};
    switch (m_type) {
        case secret::KeyringType::User:
            key_type = KEY_SPEC_USER_KEYRING;
            break;
        case secret::KeyringType::Session:
            key_type = KEY_SPEC_SESSION_KEYRING;
            break;
        case secret::KeyringType::Persistent: {
            // -1 stands for current uid, 0 stands for: do not create a link to another keyring
            auto result = keyctl_get_persistent(-1, 0);
            if (result < 0) {
                throw std::runtime_error(fmt::format("Error while getting the persistent keyring: {}", strerror(errno))
                );
            }

            this->m_ring_id = static_cast<key_serial_t>(result);

            return;
        }
        default:
            UNREACHABLE();
    }

    // 1 stands for, create if not exists, as a bool
    this->m_ring_id = keyctl_get_keyring_ID(key_type, 1);

    if (this->m_ring_id < 0) {
        throw std::runtime_error(fmt::format("Error while getting the requested keyring: {}", strerror(errno)));
    }
}

secret::SecretStorage::~SecretStorage() = default; //NOLINT(performance-trivially-destructible)

secret::SecretStorage::SecretStorage(SecretStorage&& other) noexcept
    : m_type{ other.m_type },
      m_ring_id{ other.m_ring_id } { }

[[nodiscard]] helper::expected<secret::Buffer, std::string> secret::SecretStorage::load(const std::string& key) const {

    auto key_id = get_id_from_name(m_ring_id, key);

    if (key_id < 0) {
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, can't find key by name: {}", strerror(errno))
        };
    }


    void* buffer = nullptr;

    auto result = keyctl_read_alloc(static_cast<key_serial_t>(key_id), &buffer);

    if (result < 0) {
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, can't read the value: {}", strerror(errno))
        };
    }


    auto result_buffer =
            Buffer{ reinterpret_cast<std::byte*>(buffer), //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                    static_cast<std::size_t>(result) };

    free(buffer); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
    return result_buffer;
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const Buffer& value, bool update) const {

    auto key_id = get_id_from_name(m_ring_id, key);


    if (key_id > 0 && not update) {
        return "Error while storing a key, it already exists and we can't update it";
    }

    auto full_key = get_key_name(key);

    auto new_id = add_key(constants::key_type_user, full_key.c_str(), value.data(), value.size(), m_ring_id);

    if (new_id < 0) {
        return fmt::format("Error while storing a key, can't add the key: {}", strerror(errno));
    }


    auto result = keyctl_link(new_id, m_ring_id);

    if (result < 0) {
        return fmt::format("Error while storing a key, can't link the key to the keyring: {}", strerror(errno));
    }

    return std::nullopt;
}


[[nodiscard]] std::optional<std::string> secret::SecretStorage::remove(const std::string& key) const {

    auto key_id = get_id_from_name(m_ring_id, key);

    if (key_id < 0) {
        return fmt::format("Error while removing a key, can't find key by name: {}", strerror(errno));
    }

    auto result = keyctl_unlink(static_cast<key_serial_t>(key_id), m_ring_id);

    if (result < 0) {
        return fmt::format("Error while removing a key, can't unlink key: {}", strerror(errno));
    }

    return std::nullopt;
}


#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <WTypesbase.h>
#include <ncrypt.h>


namespace {

    namespace constants {
        constexpr const wchar_t* property_name = L"OOPetris Payload";

        constexpr const wchar_t* key_name_prefix = L"OOPetris_key__";

        constexpr const wchar_t* used_algorithm = BCRYPT_AES_ALGORITHM;
    } // namespace constants

    std::wstring get_key_name(const std::string& key) {
        std::wstring result{ constants::key_name_prefix };
        for (auto& normal_char : key) {
            result += normal_char;
        }
        return result;
    }

    helper::expected<NCRYPT_KEY_HANDLE, std::string>
    get_handle_from_name(secret::KeyringType type, NCRYPT_PROV_HANDLE phProvider, const std::string& key) {


        NCRYPT_KEY_HANDLE key_handle{};

        auto key_name = get_key_name(key);

        // the key is of type user, if not specified otherwise, session mode is not supported
        DWORD flags = (type == secret::KeyringType::Persistent ? NCRYPT_MACHINE_KEY_FLAG : 0);

        // 0 means no dwLegacyKeySpec
        auto result = NCryptOpenKey(phProvider, &key_handle, key_name.c_str(), 0, flags);


        if (result != ERROR_SUCCESS) {
            return helper::unexpected<std::string>{ fmt::format("Error while opening a key: {}", result) };
        }

        return key_handle;
    }

} // namespace


secret::SecretStorage::SecretStorage(KeyringType type) : m_type{ type }, m_phProvider{} {

    if (type == KeyringType::Session) {
        spdlog::warn("KeyringType Session is not supported, using KeyringType User");
        m_type = KeyringType::User;
    }

    // there are no flags available, so using 0
    auto result = NCryptOpenStorageProvider(&this->m_phProvider, MS_KEY_STORAGE_PROVIDER, 0);

    if (result != ERROR_SUCCESS) {
        throw std::runtime_error(fmt::format("Error while getting a storage provider handle: {}", result));
    }
}

secret::SecretStorage::~SecretStorage() {
    if (m_phProvider) {
        // ignore return code, as it only indicates, if we passed a valid or invalid handle
        NCryptFreeObject(m_phProvider);
    }
}

secret::SecretStorage::SecretStorage(SecretStorage&& other) noexcept
    : m_type{ other.m_type },
      m_phProvider{ other.m_phProvider } {
    other.m_phProvider = 0;
}


[[nodiscard]] helper::expected<secret::Buffer, std::string> secret::SecretStorage::load(const std::string& key) const {

    auto maybe_key_handle = get_handle_from_name(m_type, m_phProvider, key);

    if (not maybe_key_handle.has_value()) {
        return helper::unexpected<std::string>{ maybe_key_handle.error() };
    }

    auto key_handle = maybe_key_handle.value();

    DWORD bytes_needed{};

    // no flags needed, so using 0
    auto result = NCryptGetProperty(key_handle, constants::property_name, nullptr, 0, &bytes_needed, 0);

    if (result != ERROR_SUCCESS) {
        NCryptFreeObject(key_handle);
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, getting the property size failed: {}", result)
        };
    }

    PBYTE buffer = new BYTE[bytes_needed];

    DWORD bytes_written{};

    auto result2 = NCryptGetProperty(key_handle, constants::property_name, buffer, bytes_needed, &bytes_written, 0);

    if (result2 != ERROR_SUCCESS) {
        NCryptFreeObject(key_handle);
        delete[] buffer;
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, getting the property failed: {}", result2)
        };
    }

    if (bytes_written != bytes_needed) {
        NCryptFreeObject(key_handle);
        delete[] buffer;
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, getting the property failed: mismatching sizes reported")
        };
    }

    NCryptFreeObject(key_handle);

    auto result_buffer =
            Buffer{ reinterpret_cast<std::byte*>(buffer), //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                    static_cast<std::size_t>(bytes_needed) };


    delete[] buffer;

    return result_buffer;
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const Buffer& value, bool update) const {

    NCRYPT_KEY_HANDLE key_handle{};

    auto key_name = get_key_name(key);

    // the key is of type user, if not specified otherwise, session mode is not supported, also prefer VBS, but not require it, take the update flag also in consideration
    DWORD flags = (m_type == secret::KeyringType::Persistent ? NCRYPT_MACHINE_KEY_FLAG : 0)
                  | (update ? NCRYPT_OVERWRITE_KEY_FLAG : 0)
#ifdef NCRYPT_PREFER_VBS_FLAG
                  | NCRYPT_PREFER_VBS_FLAG;
#else
            ;
#endif

    // 0 means no dwLegacyKeySpec
    auto result = NCryptCreatePersistedKey(
            this->m_phProvider, &key_handle, constants::used_algorithm, key_name.c_str(), 0, flags
    );

    if (result != ERROR_SUCCESS) {
        return fmt::format("Error while storing a key, creating a key failed: {}", result);
    }

    DWORD flags2 = m_type == KeyringType::Persistent ? NCRYPT_PERSIST_FLAG : 0;

    PBYTE buffer = new BYTE[value.size()];

    std::memcpy(buffer, value.data(), value.size());

    auto result2 =
            NCryptSetProperty(key_handle, constants::property_name, buffer, static_cast<DWORD>(value.size()), flags2);

    delete[] buffer;

    if (result2 != ERROR_SUCCESS) {
        NCryptFreeObject(key_handle);
        return fmt::format("Error while storing a key, setting the key data failed: {}", result);
    }

    // no specific flags are needed, so using 0
    auto result3 = NCryptFinalizeKey(key_handle, 0);


    if (result3 != ERROR_SUCCESS) {
        NCryptFreeObject(key_handle);
        return fmt::format("Error while storing a key, finalizing the key failed: {}", result);
    }

    // ignoring return value
    NCryptFreeObject(key_handle);
    return std::nullopt;
}


[[nodiscard]] std::optional<std::string> secret::SecretStorage::remove(const std::string& key) const {

    auto maybe_key_handle = get_handle_from_name(m_type, m_phProvider, key);

    if (not maybe_key_handle.has_value()) {
        return maybe_key_handle.error();
    }

    auto key_handle = maybe_key_handle.value();

    // no flags needed, so using 0
    auto result = NCryptDeleteKey(key_handle, 0);

    if (result != ERROR_SUCCESS) {
        NCryptFreeObject(key_handle);
        return fmt::format("Error while removing a key, deleting the key failed: {}", result);
    }

    return std::nullopt;
}

#elif defined(__CONSOLE__) || defined(__APPLE__)

#include "helper/graphic_utils.hpp"
#include <core/helper/parse_json.hpp>

namespace {

    namespace secrets_constants {
        constexpr const char* store_file_name = ".secret_key_storage";
    } // namespace secrets_constants

    helper::expected<nlohmann::json, std::string> get_json_from_file(const std::string& file) {
        auto result = json::try_parse_json_file<nlohmann::json>(file);

        if (not result.has_value()) {
            auto [error, _] = result.error();
            return helper::unexpected<std::string>{ error };
        }

        return result.value();
    }


} // namespace


// This is a dummy fallback, but good enough for this platforms
secret::SecretStorage::SecretStorage(KeyringType type) : m_type{ type } {

    m_file_path = utils::get_root_folder() / secrets_constants::store_file_name;
}

secret::SecretStorage::~SecretStorage() = default;

secret::SecretStorage::SecretStorage(SecretStorage&& other) noexcept
    : m_type{ other.m_type },
      m_file_path{ other.m_file_path } { }


[[nodiscard]] helper::expected<secret::Buffer, std::string> secret::SecretStorage::load(const std::string& key) const {

    auto maybe_json_value = get_json_from_file(m_file_path);

    if (not maybe_json_value.has_value()) {
        return helper::unexpected<std::string>{ maybe_json_value.error() };
    }

    auto json_value = maybe_json_value.value();

    if (not json_value.contains(key)) {
        return helper::unexpected<std::string>{ fmt::format("Couldn't find key: {}", key) };
    }

    std::string result{};

    json_value.at(key).get_to(result);

    return Buffer{ result };
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const Buffer& value, bool update) const {

    auto maybe_json_value = get_json_from_file(m_file_path);

    if (not maybe_json_value.has_value()) {
        return maybe_json_value.error();
    }

    auto json_value = maybe_json_value.value();

    if (json_value.contains(key) && not update) {
        return "Error while storing a key, it already exists and we can't update it";
    }


    json_value.at(key) = value.as_string();

    return json::try_write_json_to_file(m_file_path, json_value, true);
}


[[nodiscard]] std::optional<std::string> secret::SecretStorage::remove(const std::string& key) const {

    auto maybe_json_value = get_json_from_file(m_file_path);

    if (not maybe_json_value.has_value()) {
        return maybe_json_value.error();
    }

    auto json_value = maybe_json_value.value();

    json_value.erase(key);

    return json::try_write_json_to_file(m_file_path, json_value, true);
}


#elif defined(__EMSCRIPTEN__)

#include "helper/web_utils.hpp"


namespace {

    namespace constants {

        constexpr const char* key_name_prefix = "OOPetris_key__";
    } // namespace constants

    std::string get_key_name(const std::string& key) {
        return constants::key_name_prefix + key;
    }
} // namespace


// This is a dummy fallback, but good enough for this platforms
secret::SecretStorage::SecretStorage(KeyringType type) : m_type{ type } { }

secret::SecretStorage::~SecretStorage() = default;

secret::SecretStorage::SecretStorage(SecretStorage&& other) noexcept : m_type{ other.m_type } { }


[[nodiscard]] helper::expected<secret::Buffer, std::string> secret::SecretStorage::load(const std::string& key) const {

    const auto key_name = get_key_name(key);
    auto maybe_value = web::LocalStorage::get_item(key_name);
    if (not maybe_value.has_value()) {
        return helper::unexpected<std::string>{ "Key not found" };
    }

    return Buffer{ maybe_value.value() };
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const Buffer& value, bool update) const {

    const auto key_name = get_key_name(key);

    if (not update) {
        auto maybe_value = web::LocalStorage::get_item(key_name);
        if (maybe_value.has_value()) {
            return "Error while storing a key, it already exists and we can't update it";
        }
    }

    web::LocalStorage::set_item(key_name, value.as_string());

    return std::nullopt;
}


[[nodiscard]] std::optional<std::string> secret::SecretStorage::remove(const std::string& key) const {

    const auto key_name = get_key_name(key);

    web::LocalStorage::remove_item(key_name);

    return std::nullopt;
}


#else
#error "Unsupported platform"
#endif
