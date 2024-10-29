
#pragma once

#include <optional>
#include <string>

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>

#if defined(__linux__)

#include <keyutils.h>
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>

#include <windows.h>

#include <ncrypt.h>

#endif

namespace secret {

    enum class KeyringType : u8 { User, Session, Persistent };

    struct SecretStorage {
    private:
        KeyringType m_type;

#if defined(__linux__) || defined(__ANDROID__)
        key_serial_t m_ring_id;
#elif defined(__CONSOLE__) || defined(__APPLE__)
        std::string m_file_path;
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        NCRYPT_PROV_HANDLE m_phProvider;
#endif


    public:
        explicit SecretStorage(KeyringType type);

        ~SecretStorage();

        [[nodiscard]] helper::expected<std::string, std::string> load(const std::string& key) const;

        [[nodiscard]] std::optional<std::string>
        store(const std::string& key, const std::string& value, bool update = true) const;

        [[nodiscard]] std::optional<std::string> remove(const std::string& key) const;
    };

} // namespace secret
