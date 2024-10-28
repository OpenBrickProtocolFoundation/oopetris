
#pragma once

#include <optional>
#include <string>

#include <core/helper/expected.hpp>
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

        [[nodiscard]] helper::expected<std::string, std::string> load(const std::string& key) const;

        [[nodiscard]] std::optional<std::string>
        store(const std::string& key, const std::string& value, bool update = true) const;

        [[nodiscard]] std::optional<std::string> remove(const std::string& key) const;
    };

} // namespace secret
