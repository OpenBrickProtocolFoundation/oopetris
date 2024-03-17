


#if defined(_HAVE_FILE_DIALOGS)

#include "helper/expected.hpp"
#include "helper/optional.hpp"

#define NFD_THROWS_EXCEPTIONS
#include <nfd.hpp>

#include <filesystem>
#include <future>
#include <string>
#include <vector>

namespace helper {

    struct AllowedFile {
        std::string name;
        std::vector<std::string> extension_list;
    };

    [[nodiscard]] std::future<helper::expected<std::filesystem::path, std::string>> openFileDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );


    [[nodiscard]] std::future<helper::expected<std::vector<std::filesystem::path>, std::string>>
    openMultipleFilesDialog(
            const std::vector<AllowedFile>& allowed_files = {},
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

    [[nodiscard]] std::future<helper::expected<std::filesystem::path, std::string>> openFoldersDialog(
            helper::optional<std::filesystem::path> default_path = helper::nullopt
    );

} // namespace helper


#endif
