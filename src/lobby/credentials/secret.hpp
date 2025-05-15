
#pragma once

#include <optional>
#include <string>

#include <core/helper/expected.hpp>
#include <core/helper/types.hpp>

#include "./buffer.hpp"
#include "helper/windows.hpp"
#include "manager/service_provider.hpp"

#if defined(__linux__)

#include <keyutils.h>
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <basetsd.h>
namespace oopetris::secret::details {
    using NCRYPT_PROV_HANDLE = ULONG_PTR;
} // namespace oopetris::secret::details

#endif

namespace secret {

    enum class KeyringType : u8 { User, Session, Persistent };

    struct SecretStorage {
    private:
        [[maybe_unused]] ServiceProvider* m_service_provider;
        KeyringType m_type;

#if defined(__linux__) || defined(__ANDROID__)
        key_serial_t m_ring_id;
#elif defined(__CONSOLE__) || defined(__APPLE__)
        std::string m_file_path;
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        oopetris::secret::details::NCRYPT_PROV_HANDLE m_phProvider;
#endif


    public:
        OOPETRIS_GRAPHICS_EXPORTED explicit SecretStorage(ServiceProvider* service_provider, KeyringType type);

        OOPETRIS_GRAPHICS_EXPORTED ~SecretStorage(); //NOLINT(performance-trivially-destructible)

        SecretStorage(const SecretStorage& other) = delete;
        SecretStorage& operator=(const SecretStorage& other) = delete;

        OOPETRIS_GRAPHICS_EXPORTED SecretStorage(SecretStorage&& other) noexcept;
        SecretStorage& operator=(SecretStorage&& other) noexcept = delete;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] helper::expected<Buffer, std::string> load(const std::string& key
        ) const;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string>
        store(const std::string& key, const Buffer& value, bool update = true) const;

        OOPETRIS_GRAPHICS_EXPORTED [[nodiscard]] std::optional<std::string> remove(const std::string& key) const;
    };

} // namespace secret
