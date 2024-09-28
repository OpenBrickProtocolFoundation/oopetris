

#pragma once

#include <core/helper/expected.hpp>

#include <filesystem>
#include <functional>
#include <string>

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

namespace dynamic_loader {

    struct Library {
    private:
#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        using HandleType = HMODULE;
#else
        using HandleType = void*;
#endif

        HandleType m_handle;

        explicit Library(HandleType handle);

    public:
        ~Library() noexcept;

        static helper::expected<Library, std::string> load(const std::filesystem::path& library_path);
        static helper::expected<Library, std::string> load(const std::string& library_name);


        template<typename T>
        helper::expected<std::function<T>, std::string> resolve_symbol(const std::string& name) const {

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

            FARPROC symbol = GetProcAddress(m_handle, name.c_str());

            if (symbol == nullptr) {
                return helper::unexpected<std::string>{ std::string{ GetLastError() } };
            }

#else
            void* const symbol = dlsym(m_handle, name.c_str());

            if (symbol == nullptr) {
                return helper::unexpected<std::string>{ std::string{ dlerror() } };
            }

#endif


            return reinterpret_cast<std::function<T>>(symbol); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        }
    };

} // namespace dynamic_loader
