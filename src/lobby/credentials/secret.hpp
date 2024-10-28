
#pragma once

#include <optional>
#include <string>

#include <core/helper/types.hpp>

#if defined(__linux__)

#include <keyutils.h>

#endif

namespace secret {

    enum class KeyringType : u8 { User, Session, Persistent };

    struct SecretStorage {
    private:
        KeyringType m_type;

#if defined(__linux__)
        key_serial_t m_ring_id;
#endif


    public:
        explicit SecretStorage(KeyringType type);

        [[nodiscard]] std::optional<std::string> load(const std::string& key) const;

        void store(const std::string& key, const std::string& value, bool update = true) const;

        void remove(const std::string& key) const;
    };

} // namespace secret
