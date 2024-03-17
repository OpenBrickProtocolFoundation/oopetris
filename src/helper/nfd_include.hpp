


#if defined(_HAVE_FILE_DIALOGS)

#include "helper/expected.hpp"
#include "helper/optional.hpp"

#define NFD_THROWS_EXCEPTIONS
#include <nfd.hpp>

#include <filesystem>
#include <future>
#include <string>
#include <vector>

namespace NFD {

#ifdef _WIN32
    /* denotes UTF-16 char */
    using string = std::wstring;
#else
    using string = std::string;
#endif // _WIN32

} // namespace NFD


namespace helper {


    struct AllowedFile {
        NFD::string name;
        std::vector<NFD::string> extension_list;
    };

    [[nodiscard]] std::future<helper::expected<std::filesystem::path, NFD::string>> openFileDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );


    [[nodiscard]] std::future<helper::expected<std::vector<std::filesystem::path>, NFD::string>>
    openMultipleFilesDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

    [[nodiscard]] std::future<helper::expected<std::filesystem::path, NFD::string>> openFoldersDialog(
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

} // namespace helper


#endif
