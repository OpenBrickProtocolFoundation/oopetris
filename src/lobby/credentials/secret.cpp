
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

    key_serial_t get_id_from_name(key_serial_t keyring_id, const std::string& key) {

        std::string full_key = get_key_name(key);

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
            this->m_ring_id = keyctl_get_persistent(-1, 0);
            if (this->m_ring_id < 0) {
                throw std::runtime_error(fmt::format("Error while getting the persistent keyring: {}", strerror(errno))
                );
            }

            return;
        }
        default:
            UNREACHABLE();
    }

    // 1 stands for, create if not exists, as a bool
    this->m_ring_id = keyctl_get_keyring_ID(key_type, 1);

    if (m_ring_id < 0) {
        throw std::runtime_error(fmt::format("Error while getting the requested keyring: {}", strerror(errno)));
    }
}


[[nodiscard]] helper::expected<std::string, std::string> secret::SecretStorage::load(const std::string& key) const {

    if (m_ring_id < 0) {
        return helper::unexpected<std::string>{ "Error while loading a key, ring_id is invalid" };
    }

    auto key_id = get_id_from_name(m_ring_id, key);

    if (key_id < 0) {
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, can't find key by name: {}", strerror(errno))
        };
    }


    void* buffer = nullptr;

    auto result = keyctl_read_alloc(key_id, &buffer);

    if (result < 0) {
        return helper::unexpected<std::string>{
            fmt::format("Error while loading a key, can't read the value: {}", strerror(errno))
        };
    }

    auto result_string = std::string{ static_cast<char*>(buffer) };
    free(buffer);
    return result_string;
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const std::string& value, bool update) const {

    if (m_ring_id < 0) {
        return "Error while storing a key, ring_id is invalid";
    }

    auto key_id = get_id_from_name(m_ring_id, key);


    if (key_id > 0 && not update) {
        return "Error while storing a key, it already exists and we can't update it";
    }

    auto full_key = get_key_name(key);

    auto new_id = add_key(constants::key_type_user, full_key.c_str(), value.c_str(), value.size(), m_ring_id);

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

    if (m_ring_id < 0) {
        return fmt::format("Error while remove a key, ring_id is invalid");
    }

    auto key_id = get_id_from_name(m_ring_id, key);

    if (key_id < 0) {
        return fmt::format("Error while removing a key, can't find key by name: {}", strerror(errno));
    }

    auto result = keyctl_unlink(key_id, m_ring_id);

    if (result < 0) {
        return fmt::format("Error while removing a key, can't unlink key: {}", strerror(errno));
    }

    return std::nullopt;
}


#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)


//TODO: use https://learn.microsoft.com/en-us/windows/win32/seccng/cng-key-storage-functions

secret::SecretStorage::SecretStorage(KeyringType type) : m_type{ type } { }


[[nodiscard]] helper::expected<std::string, std::string> secret::SecretStorage::load(const std::string& key) const {
    return result_string;
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const std::string& value, bool update) const {
    return std::nullopt;
}


[[nodiscard]] std::optional<std::string> secret::SecretStorage::remove(const std::string& key) const {
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
            return helper::unexpected<std::string>{ result.error() };
        }

        return result.value();
    }


} // namespace


// This is a dummy fallback, but good enough for this platforms
secret::SecretStorage::SecretStorage(KeyringType type) : m_type{ type } {

    m_file_path = utils::get_root_folder() / secrets_constants::store_file_name;
}


[[nodiscard]] helper::expected<std::string, std::string> secret::SecretStorage::load(const std::string& key) const {

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

    return result;
}

[[nodiscard]] std::optional<std::string>
secret::SecretStorage::store(const std::string& key, const std::string& value, bool update) const {

    auto maybe_json_value = get_json_from_file(m_file_path);

    if (not maybe_json_value.has_value()) {
        return maybe_json_value.error();
    }

    auto json_value = maybe_json_value.value();

    if (json_value.contains(key) && not update) {
        return "Error while storing a key, it already exists and we can't update it";
    }


    json_value.at(key) = value;

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


#else
#error "Unsupported platform"
#endif
