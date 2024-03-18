


#if defined(_HAVE_FILE_DIALOGS)

#include "helper/expected.hpp"
#include "helper/optional.hpp"

#define NFD_THROWS_EXCEPTIONS
#include <nfd.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace NFD {

#ifdef _WIN32
    /* denotes UTF-16 char */
    using string = std::wstring;
#define NFD_CHAR(str) L##str
#else
    using string = std::string;
#define NFD_CHAR(str) str
#endif // _WIN32


} // namespace NFD


namespace helper {


    struct AllowedFile {
        NFD::string name;
        std::vector<NFD::string> extension_list;
    };


    //NOTE: this API is blocking and can't be asynchronous, due to os (linux, windows, macos) restrictions, it HAS to be launched in the same thread NFD_Init() was launched /the main thread)
    [[nodiscard]] helper::expected<std::filesystem::path, NFD::string> openFileDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

    [[nodiscard]] helper::expected<std::vector<std::filesystem::path>, NFD::string> openMultipleFilesDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

    [[nodiscard]] helper::expected<std::filesystem::path, NFD::string> openFolderDialog(
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

} // namespace helper


#endif
