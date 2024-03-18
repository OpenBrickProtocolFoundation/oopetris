


#if defined(_HAVE_FILE_DIALOGS)

#include "helper/expected.hpp"
#include "helper/optional.hpp"

#define NFD_THROWS_EXCEPTIONS
#define NFD_DIFFERENT_NATIVE_FUNCTIONS
#include <nfd.hpp>

#include <filesystem>
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
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

    [[nodiscard]] helper::expected<std::vector<std::filesystem::path>, std::string> openMultipleFilesDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

    [[nodiscard]] helper::expected<std::filesystem::path, std::string> openFolderDialog(
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

} // namespace helper


#endif
