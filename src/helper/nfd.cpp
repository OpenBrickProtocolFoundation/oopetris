
#if defined(_HAVE_FILE_DIALOGS)

#include "nfd_include.hpp"


std::future<helper::expected<std::filesystem::path, std::string>> helper::openFileDialog(
        std::vector<helper::AllowedFile> allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(std::launch::async, [&allowed_files, &default_path] {
        //TODO: test if this get's freed correctly!
        UniquePath outPath{};
        nfdfilteritem_t* filterItem = nullptr;
        if (not allowed_files.empty()) {
            filterItem = malloc(sizeof(nfdfilteritem_t*) * allowed_files.size());
            for (const auto& allowed_file : allowed_files) {
                filterItem[i] = { allowed_file.name.c_str(), allowed_file.extension_list.c_str() };
            }
        }

        const nfdnchar_t* default_path_C = nullptr;
        if (default_path.has_value()) {
            default_path_C = default_path.value().string().c_str();
        }

        nfdresult_t result = NFD::OpenDialog(outPath.get(), filterItem, allowed_files.size(), default_path_C);
        if (result == NFD_OKAY) {
            return std::filesystem::path{ *outPath.get() };
        } else if (result == NFD_CANCEL) {
            return helper::unexpected<std::string>{ "The user pressed cancel." };
        } else {
            return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
        }
    });
}


#endif
