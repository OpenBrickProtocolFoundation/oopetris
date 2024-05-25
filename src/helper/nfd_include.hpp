


#if defined(_HAVE_FILE_DIALOGS)

#include <core/helper/expected.hpp>


#define NFD_THROWS_EXCEPTIONS
#ifdef _WIN32
#define NFD_DIFFERENT_NATIVE_FUNCTIONS
#endif
#include <nfd.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace helper {


    struct AllowedFile {
        std::string name;
        std::vector<std::string> extension_list;
    };


    //NOTE: this API is blocking and can't be asynchronous, due to os (linux, windows, macos) restrictions, it HAS to be launched in the same thread NFD_Init() was launched /the main thread)
    [[nodiscard]] helper::expected<std::filesystem::path, std::string> openFileDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            std::optional<std::filesystem::path> default_path = std::nullopt
    );

    [[nodiscard]] helper::expected<std::vector<std::filesystem::path>, std::string> openMultipleFilesDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            std::optional<std::filesystem::path> default_path = std::nullopt
    );

    [[nodiscard]] helper::expected<std::filesystem::path, std::string> openFolderDialog(
            std::optional<std::filesystem::path> default_path = std::nullopt
    );

} // namespace helper


#endif
