
#include "./secret.hpp"

#if defined(__linux__)

#include <core/helper/utils.hpp>
#include <fmt/format.h>
namespace {

    namespace constants {
        constexpr const char* key_type_user = "user";

        constexpr const std::string key_name_prefix = "OOPetris_key__";
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
        return "Error while storing a key, it already exists and can't update it";
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


#elif defined(__ANDROID__)
//TODO: use https://developer.android.com/privacy-and-security/keystore

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

#elif defined(__CONSOLE__)

//TODO: fall back to just store it in a file, that is json
#error "CURRENTLY Unsupported platform"

#elif defined(__APPLE__)

//TODO: fall back to just store it in a file, that is json
#error "CURRENTLY Unsupported platform"
#else
#error "Unsupported platform"
#endif
