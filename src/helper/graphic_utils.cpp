
#include "graphic_utils.hpp"
#include <exception>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

SDL_Color utils::sdl_color_from_color(const Color& color) {
    return SDL_Color{ color.r, color.g, color.b, color.a };
}


std::vector<std::string> utils::supported_features() {

    std::vector<std::string> features{};

    features.emplace_back("online multiplayer");

#if defined(_HAVE_FILE_DIALOGS)
    features.emplace_back("file dialogs");
#endif

#if defined(_HAVE_DISCORD_SDK)
    features.emplace_back("discord integration");
#endif

#if defined(_ENABLE_REPLAY_RENDERING)
#if defined(_FFMPEG_USE_EMBEDDED)
    features.emplace_back("replay video rendering (embedded)");
#else
    features.emplace_back("replay video rendering (system)");
#endif
#endif

    return features;
}

/**
 * \brief this returns the directory of the app, it's read and write-able
*/
[[nodiscard]] std::filesystem::path utils::get_root_folder() {
#if defined(__ANDROID__)
    // this call also creates the dir (at least tries to) it returns
    char* pref_path = SDL_GetPrefPath(constants::author, constants::program_name);
    if (!pref_path) {
        throw std::runtime_error{ "Failed in getting the Pref Path: " + std::string{ SDL_GetError() } };
    }
    return std::filesystem::path{ std::string{ pref_path } };
#elif defined(__EMSCRIPTEN__)
    return std::filesystem::path{ "/" };
#elif defined(__CONSOLE__)
    // this is in the sdcard of the switch / 3ds , since internal storage is read-only for applications!
    return std::filesystem::path{ "." };
#elif defined(BUILD_INSTALLER)
#if defined(FLATPAK_BUILD)
    // this is a read write location in the flatpak build, see https://docs.flatpak.org/en/latest/conventions.html
    const char* data_home = std::getenv("XDG_DATA_HOME");
    if (data_home == nullptr) {
        throw std::runtime_error{ "Failed to get flatpak data directory (XDG_DATA_HOME)" };
    }

    return std::filesystem::path{ data_home } / "share" / constants::program_name;
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    char* pref_path = SDL_GetPrefPath(constants::author, constants::program_name);
    if (!pref_path) {
        throw std::runtime_error{ "Failed in getting the Pref Path: " + std::string{ SDL_GetError() } };
    }
    return std::filesystem::path{ pref_path };
#else
#error "unrecognized installer build"
#endif
#elif defined(INSTALL_FILES)
#if !defined(INSTALL_LOCATION)
#error "missing 'INSTALL_LOCATION' define"
#endif

#if defined(__SERENITY__)


    // this is a read write location in the serenity-os case build, see https://docs.flatpak.org/en/latest/conventions.html
    const char* data_home = std::getenv("HOME");
    if (data_home == nullptr) {
        throw std::runtime_error{ "Failed to get flatpak data directory (XDG_DATA_HOME)" };
    }

    return std::filesystem::path{ data_home } / "share" / constants::program_name;

#else

#define STRINGIFY(a) STRINGIFY_HELPER_(a) //NOLINT(cppcoreguidelines-macro-usage)
#define STRINGIFY_HELPER_(a) #a

    return std::filesystem::path{ STRINGIFY(INSTALL_LOCATION) } / "share" / constants::program_name;


#undef STRINGIFY
#undef STRINGIFY_HELPER_
#endif
#else
    // this is only used in local build for debugging, when compiling in release mode the path is real path where the app can store many things without interfering with other things (eg. AppData\Roaming\... on Windows or  .local/share/... on Linux )
    return std::filesystem::path{ "." };
#endif
}

/**
 * \brief this returns the directory of the assets, and is read-only 
*/
[[nodiscard]] std::filesystem::path utils::get_assets_folder() {
#if defined(__ANDROID__)
    return std::filesystem::path{ "" };
#elif defined(__EMSCRIPTEN__)
    // emscripten mounts a memfs in the / location, we package assest into this dir, see: https://emscripten.org/docs/porting/files/packaging_files.html#packaging-using-emcc
    return std::filesystem::path{ "/assets" };
#elif defined(__CONSOLE__)
    // this is in the internal storage of the nintendo switch, it is mounted by libnx (runtime switch support library) and filled at compile time with assets (its called ROMFS there)
    return std::filesystem::path{ "romfs:/assets" };
#elif defined(BUILD_INSTALLER)
    // if you build in BUILD_INSTALLER mode, you have to assure that the data is there e.g. music  + fonts!
#if defined(FLATPAK_BUILD)
    return std::filesystem::path{ "/app/share/oopetris/assets/" };
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    char* resource_path = SDL_GetBasePath();
    if (!resource_path) {
        throw std::runtime_error{ "Failed in getting the Base Path: " + std::string{ SDL_GetError() } };
    }
    return std::filesystem::path{ resource_path } / "assets";
#else
#error "unrecognized installer build"
#endif
#elif defined(INSTALL_FILES)
#if !defined(INSTALL_LOCATION)
#error "missing 'INSTALL_LOCATION' define"
#endif

#define STRINGIFY(a) STRINGIFY_HELPER_(a) //NOLINT(cppcoreguidelines-macro-usage)
#define STRINGIFY_HELPER_(a) #a

    return std::filesystem::path{ STRINGIFY(INSTALL_LOCATION) } / "share" / "oopetris" / "assets";


#undef STRINGIFY
#undef STRINGIFY_HELPER_

#else


    return std::filesystem::path{ "assets" };
#endif
}

std::optional<bool> utils::log_error(const std::string& error) {
    spdlog::error(error);
    return std::nullopt;
}

utils::ExitException::ExitException(int status_code) noexcept : m_status_code{ status_code } { }

[[nodiscard]] int utils::ExitException::status_code() const {
    return m_status_code;
}

[[nodiscard]] const char* utils::ExitException::what() const noexcept {
    return "An exit exception occurred";
}

std::optional<std::string> utils::create_directory(const std::filesystem::path& folder, bool recursive) {

    if (std::filesystem::exists(folder)) {
        return std::nullopt;
    }

    try {
        if (recursive) {
            auto result = std::filesystem::create_directories(folder);
            if (not result) {
                return "an unknown error occurred";
            }
            return std::nullopt;
        }


        auto result = std::filesystem::create_directory(folder);
        if (not result) {
            return "an unknown error occurred";
        }
        return std::nullopt;
    } catch (const std::exception& error) {
        return error.what();
    }
}

void utils::exit(int status_code) {
#if defined(__ANDROID__)
    // calling exit() in android doesn't do the correct job, it completely avoids resource cleanup by the underlying SDLActivity.java
    // (java wrapper), that calls the main and expects it to return ALWAYS and throwing an exception in a catch statement is bad,
    // but is required here
    // see: https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md
    throw utils::ExitException{ status_code };
#elif defined(__EMSCRIPTEN__)
    emscripten_force_exit(status_code);
#else
    std::exit(status_code);
#endif
}

// inspired by SDL_SYS_SetupThread also uses that code for most platforms
OOPETRIS_GRAPHICS_EXPORTED void utils::set_thread_name(const char* name) {

#if defined(__APPLE__) || defined(__MACOSX__)
    if (pthread_setname_np(name) == ERANGE) {
        char namebuf[16] = {}; /* Limited to 16 chars (with 0 byte) */
        memcpy(namebuf, name, 15);
        namebuf[15] = '\0';
        pthread_setname_np(namebuf);
    }
#elif defined(__linux__) || defined(__ANDROID__) || defined(FLATPAK_BUILD)
    if (pthread_setname_np(pthread_self(), name) == ERANGE) {
        char namebuf[16] = {}; /* Limited to 16 chars (with 0 byte) */
        memcpy(namebuf, name, 15);
        namebuf[15] = '\0';
        pthread_setname_np(pthread_self(), namebuf);
    }
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    std::wstring name_w{};
    for (std::size_t i = 0; i < strlen(name); ++i) {
        name_w += name[i];
    }

    SetThreadDescription(GetCurrentThread(), name_w.c_str());

#else
    UNUSED(name);
#endif
}
