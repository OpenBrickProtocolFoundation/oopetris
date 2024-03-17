
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
    template<typename T>
    using UniquePtr = std::unique_ptr<T, std::function<void(const T* const)>>;


    [[nodiscard]] FilterItemType get_filter_items(const std::vector<helper::AllowedFile>& allowed_files) {

        FilterItemType filterItem{ allowed_files.empty() ? nullptr : new nfdfilteritem_t[allowed_files.size()],
                                   [](const nfdfilteritem_t* const value) {
                                       if (value == nullptr) {
                                           return;
                                       }

                                       delete[] value; // NOLINT(cppcoreguidelines-owning-memory)
                                   } };

        std::vector<NFD::string> extensions_composed{};
        extensions_composed.reserve(allowed_files.size());
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


std::future<helper::expected<std::filesystem::path, NFD::string>> helper::openFileDialog(
        const std::vector<AllowedFile>& allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(
            std::launch::async,
            [&allowed_files, &default_path] -> helper::expected<std::filesystem::path, NFD::string> {
                NFD::UniquePath outPath{};
                auto filterItem = get_filter_items(allowed_files);

                const auto path_deallocator = [](const nfdnchar_t* const char_value) {
                    if (char_value == nullptr) {
                        return;
                    }

                    delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
                };

                UniquePtr<nfdnchar_t> default_path_value{ nullptr, path_deallocator };

                if (default_path.has_value()) {
                    const auto& str = default_path.value().string();
                    const auto str_size = str.size() + 1;
                    default_path_value = UniquePtr<nfdnchar_t>{ new nfdnchar_t[str_size], path_deallocator };
                    std::memcpy(default_path_value.get(), str.c_str(), str_size * sizeof(nfdnchar_t));
                }

                const nfdresult_t result =
                        NFD::OpenDialog(outPath, filterItem.get(), allowed_files.size(), default_path_value.get());
                if (result == NFD_OKAY) {
                    return std::filesystem::path{ outPath.get() };
                }

                if (result == NFD_CANCEL) {
                    return helper::unexpected<NFD::string>{ "The user pressed cancel." };
                }

                return helper::unexpected<NFD::string>{ "Error: " + NFD::string{ NFD::GetError() } };
            }
    );
}


[[nodiscard]] std::future<helper::expected<std::vector<std::filesystem::path>, NFD::string>>
helper::openMultipleFilesDialog(
        const std::vector<AllowedFile>& allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(
            std::launch::async,
            [&allowed_files, &default_path] -> helper::expected<std::vector<std::filesystem::path>, NFD::string> {
                NFD::UniquePathSet outPaths{};
                auto filterItem = get_filter_items(allowed_files);

                const auto path_deallocator = [](const nfdnchar_t* const char_value) {
                    if (char_value == nullptr) {
                        return;
                    }

                    delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
                };

                UniquePtr<nfdnchar_t> default_path_value{ nullptr, path_deallocator };

                if (default_path.has_value()) {
                    const auto& str = default_path.value().string();
                    const auto str_size = str.size() + 1;
                    default_path_value = UniquePtr<nfdnchar_t>{ new nfdnchar_t[str_size], path_deallocator };
                    std::memcpy(default_path_value.get(), str.c_str(), str_size * sizeof(nfdnchar_t));
                }

                const nfdresult_t result = NFD::OpenDialogMultiple(
                        outPaths, filterItem.get(), allowed_files.size(), default_path_value.get()
                );
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
                }

                if (result == NFD_CANCEL) {
                    return helper::unexpected<NFD::string>{ "The user pressed cancel." };
                }

                return helper::unexpected<NFD::string>{ "Error: " + NFD::string{ NFD::GetError() } };
            }
    );
}

[[nodiscard]] std::future<helper::expected<std::filesystem::path, NFD::string>> helper::openFoldersDialog(
        helper::optional<std::filesystem::path> default_path
) {

    return std::async(std::launch::async, [&default_path] -> helper::expected<std::filesystem::path, NFD::string> {
        NFD::UniquePath outPath{};

        const auto path_deallocator = [](const nfdnchar_t* const char_value) {
            if (char_value == nullptr) {
                return;
            }

            delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
        };


        UniquePtr<nfdnchar_t> default_path_value{ nullptr, path_deallocator };

        if (default_path.has_value()) {
            const auto& str = default_path.value().string();
            const auto str_size = str.size() + 1;
            default_path_value = UniquePtr<nfdnchar_t>{ new nfdnchar_t[str_size], path_deallocator };
            std::memcpy(default_path_value.get(), str.c_str(), str_size * sizeof(nfdnchar_t));
        }

        const nfdresult_t result = NFD::PickFolder(outPath, default_path_value.get());
        if (result == NFD_OKAY) {
            return std::filesystem::path{ outPath.get() };
        }

        if (result == NFD_CANCEL) {
            return helper::unexpected<NFD::string>{ "The user pressed cancel." };
        }

        return helper::unexpected<NFD::string>{ "Error: " + NFD::string{ NFD::GetError() } };
    });
}


#endif
