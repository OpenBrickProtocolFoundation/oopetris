

#include "loader.hpp"

#include <iostream>


dynamic_loader::Library::Library(Library::HandleType handle) : m_handle{ handle } { }

dynamic_loader::Library::~Library() noexcept {

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    BOOL result = FreeLibrary(this->m_handle);

    if (result == 0) {
        std::cerr << "Error in FreeLibrary(): " << std::string{ GetLastError() } << "\n";
    }
#else

    int result = dlclose(this->m_handle);

    if (result != 0) {
        std::cerr << "Error in dlclose(): " << std::string{ dlerror() } << "\n";
    }

#endif
}

helper::expected<dynamic_loader::Library, std::string> dynamic_loader::Library::load(
        const std::filesystem::path& library_path
) {
    if (not std::filesystem::exists(library_path)) {
        return helper::unexpected<std::string>{ "Path does not exist" };
    }

    return load(library_path.string());
}


helper::expected<dynamic_loader::Library, std::string> dynamic_loader::Library::load(const std::string& library_name) {

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    HandleType handle = LoadLibraryA(library_name.c_str());

    if (handle == nullptr) {
        return helper::unexpected<std::string>{ std::string{ GetLastError() } };
    }
#else

    HandleType handle = dlopen(library_name.c_str(), RTLD_LAZY);

    if (handle == nullptr) {
        return helper::unexpected<std::string>{ std::string{ dlerror() } };
    }

#endif

    return Library{ handle };
}
