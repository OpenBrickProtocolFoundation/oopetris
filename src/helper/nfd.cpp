
#if defined(_HAVE_FILE_DIALOGS)

#include "helper/types.hpp"
#include "nfd_include.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <functional>
#include <memory>

std::future<helper::expected<std::filesystem::path, std::string>> helper::openFileDialog(
        std::vector<helper::AllowedFile> allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(
            std::launch::async,
            [&allowed_files, &default_path] -> helper::expected<std::filesystem::path, std::string> {
                //TODO: BEFORE PR  test if this get's freed correctly!
                NFD::UniquePath outPath{};
                std::unique_ptr<nfdfilteritem_t, std::function<void(const nfdfilteritem_t* const)>> filterItem{
                    allowed_files.empty() ? nullptr : new nfdfilteritem_t[allowed_files.size()],
                    [](const nfdfilteritem_t* const value) {
                        if (value == nullptr) {
                            return;
                        }

                        delete[] value; // NOLINT(cppcoreguidelines-owning-memory)
                    }
                };

                std::vector<std::string> extensions_composed{};
                for (const auto& allowed_file : allowed_files) {
                    extensions_composed.emplace_back(fmt::format("{}", fmt::join(allowed_file.extension_list, ",")));
                }

                if (not allowed_files.empty()) {
                    for (usize i = 0; i < allowed_files.size(); ++i) {
                        const auto& allowed_file = allowed_files.at(i);
                        filterItem.get()[i] = { allowed_file.name.c_str(), extensions_composed.at(i).c_str() };
                    }
                }

                const nfdnchar_t* default_path_C = nullptr;
                if (default_path.has_value()) {
                    default_path_C = default_path.value().string().c_str();
                }

                nfdresult_t result = NFD::OpenDialog(outPath, filterItem.get(), allowed_files.size(), default_path_C);
                if (result == NFD_OKAY) {
                    return std::filesystem::path{ outPath.get() };
                } else if (result == NFD_CANCEL) {
                    return helper::unexpected<std::string>{ "The user pressed cancel." };
                } else {
                    return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
                }
            }
    );
}


#endif
