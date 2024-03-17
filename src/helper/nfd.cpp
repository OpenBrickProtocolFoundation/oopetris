
#include "nfd.hpp"
#include "helper/utils.hpp"
#if defined(_HAVE_FILE_DIALOGS)

#include "helper/types.hpp"
#include "nfd_include.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <functional>
#include <memory>

namespace {

    using FilterItemType = std::unique_ptr<nfdfilteritem_t, std::function<void(const nfdfilteritem_t* const)>>;


    [[nodiscard]] FilterItemType get_filter_items(const std::vector<helper::AllowedFile>& allowed_files) {

        FilterItemType filterItem{ allowed_files.empty() ? nullptr : new nfdfilteritem_t[allowed_files.size()],
                                   [](const nfdfilteritem_t* const value) {
                                       if (value == nullptr) {
                                           return;
                                       }

                                       delete[] value; // NOLINT(cppcoreguidelines-owning-memory)
                                   } };

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

        return filterItem;

    } // namespace


} // namespace


std::future<helper::expected<std::filesystem::path, std::string>> helper::openFileDialog(
        const std::vector<AllowedFile>& allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(
            std::launch::async,
            [&allowed_files, &default_path] -> helper::expected<std::filesystem::path, std::string> {
                NFD::UniquePath outPath{};
                auto filterItem = get_filter_items(allowed_files);

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


[[nodiscard]] std::future<helper::expected<std::vector<std::filesystem::path>, std::string>>
helper::openMultipleFilesDialog(
        const std::vector<AllowedFile>& allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(
            std::launch::async,
            [&allowed_files, &default_path] -> helper::expected<std::vector<std::filesystem::path>, std::string> {
                NFD::UniquePathSet outPaths{};
                auto filterItem = get_filter_items(allowed_files);

                const nfdnchar_t* default_path_C = nullptr;
                if (default_path.has_value()) {
                    default_path_C = default_path.value().string().c_str();
                }

                nfdresult_t result =
                        NFD::OpenDialogMultiple(outPaths, filterItem.get(), allowed_files.size(), default_path_C);
                if (result == NFD_OKAY) {
                    std::vector<std::filesystem::path> result_vector{};

                    nfdpathsetsize_t count_paths{};
                    const auto temp_result = NFD::PathSet::Count(outPaths, count_paths);
                    ASSERT(temp_result == NFD_OKAY && "PathSet get count is successful");

                    for (nfdpathsetsize_t i = 0; i < count_paths; ++i) {
                        NFD::UniquePathSetPath outPath{};
                        const auto temp_result2 = NFD::PathSet::GetPath(outPaths, i, outPath);
                        ASSERT(temp_result2 == NFD_OKAY && "PathSet get path is successful");
                        result_vector.emplace_back(outPath.get());
                    }

                    return result_vector;
                } else if (result == NFD_CANCEL) {
                    return helper::unexpected<std::string>{ "The user pressed cancel." };
                } else {
                    return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
                }
            }
    );
}

[[nodiscard]] std::future<helper::expected<std::filesystem::path, std::string>> helper::openFoldersDialog(
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(std::launch::async, [&default_path] -> helper::expected<std::filesystem::path, std::string> {
        NFD::UniquePath outPath{};

        const nfdnchar_t* default_path_C = nullptr;
        if (default_path.has_value()) {
            default_path_C = default_path.value().string().c_str();
        }

        nfdresult_t result = NFD::PickFolder(outPath, default_path_C);
        if (result == NFD_OKAY) {
            return std::filesystem::path{ outPath.get() };
        } else if (result == NFD_CANCEL) {
            return helper::unexpected<std::string>{ "The user pressed cancel." };
        } else {
            return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
        }
    });
}


#endif
