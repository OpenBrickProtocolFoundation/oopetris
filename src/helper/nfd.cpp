
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

    using FilterItemType = std::unique_ptr<nfdu8filteritem_t, std::function<void(const nfdu8filteritem_t* const)>>;
    template<typename T>
    using UniquePtr = std::unique_ptr<T, std::function<void(const T* const)>>;

    [[nodiscard]] FilterItemType get_filter_items(const std::vector<helper::AllowedFile>& allowed_files) {
        const auto size = static_cast<nfdfiltersize_t>(allowed_files.size());
        FilterItemType filter_item{ allowed_files.empty() ? nullptr : new nfdu8filteritem_t[size],
                                    [size](const nfdu8filteritem_t* const value) {
                                        if (value == nullptr) {
                                            return;
                                        }

                                        for (usize i = 0; i < size; ++i) {
                                            const auto& item =
                                                    value[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                                            delete item.name; // NOLINT(cppcoreguidelines-owning-memory)
                                            delete item.spec; // NOLINT(cppcoreguidelines-owning-memory)
                                        }

                                        delete[] value; // NOLINT(cppcoreguidelines-owning-memory)
                                    } };


        if (not allowed_files.empty()) {

            for (usize i = 0; i < static_cast<nfdfiltersize_t>(allowed_files.size()); ++i) {
                const auto& allowed_file = allowed_files.at(i);

                const auto& filter_name = allowed_file.name;
                const auto filter_name_size = filter_name.size() + 1;

                auto* name = new nfdu8char_t[filter_name_size]; // NOLINT(cppcoreguidelines-owning-memory)
                std::memcpy(name, filter_name.c_str(), filter_name_size * sizeof(nfdu8char_t));

                const std::string extension_list = fmt::format("{}", fmt::join(allowed_file.extension_list, ","));
                const auto extension_list_size = extension_list.size() + 1;

                auto* extensions = new nfdu8char_t[extension_list_size]; // NOLINT(cppcoreguidelines-owning-memory)
                std::memcpy(extensions, extension_list.c_str(), extension_list_size * sizeof(nfdu8char_t));

                filter_item.get()[i] = { name, extensions };
            }
        }

        return filter_item;

    } // namespace


} // namespace


helper::expected<std::filesystem::path, std::string> helper::openFileDialog(
        const std::vector<AllowedFile>& allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    NFD::UniquePathU8 out_path{};
    auto filterItem = get_filter_items(allowed_files);

    const auto path_deallocator = [](const nfdu8char_t* const char_value) {
        if (char_value == nullptr) {
            return;
        }

        delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
    };

    UniquePtr<nfdu8char_t> default_path_value{ nullptr, path_deallocator };

    if (default_path.has_value()) {
        const auto& str = default_path.value().string();
        const auto str_size = str.size() + 1;
        default_path_value = UniquePtr<nfdu8char_t>{ new nfdu8char_t[str_size], path_deallocator };
        std::memcpy(default_path_value.get(), str.c_str(), str_size * sizeof(nfdu8char_t));
    }

    const nfdresult_t result = NFD::OpenDialog(
            out_path, filterItem.get(), static_cast<nfdfiltersize_t>(allowed_files.size()), default_path_value.get()
    );
    if (result == NFD_OKAY) {
        return std::filesystem::path{ out_path.get() };
    }

    if (result == NFD_CANCEL) {
        return helper::unexpected<std::string>{ "The user pressed cancel." };
    }

    return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
}


[[nodiscard]] helper::expected<std::vector<std::filesystem::path>, std::string> helper::openMultipleFilesDialog(
        const std::vector<AllowedFile>& allowed_files,
        helper::optional<std::filesystem::path> default_path
) {

    NFD::UniquePathSet out_paths{};
    auto filterItem = get_filter_items(allowed_files);

    const auto path_deallocator = [](const nfdu8char_t* const char_value) {
        if (char_value == nullptr) {
            return;
        }

        delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
    };

    UniquePtr<nfdu8char_t> default_path_value{ nullptr, path_deallocator };

    if (default_path.has_value()) {
        const auto& str = default_path.value().string();
        const auto str_size = str.size() + 1;
        default_path_value = UniquePtr<nfdu8char_t>{ new nfdu8char_t[str_size], path_deallocator };
        std::memcpy(default_path_value.get(), str.c_str(), str_size * sizeof(nfdu8char_t));
    }

    const nfdresult_t result = NFD::OpenDialogMultiple(
            out_paths, filterItem.get(), static_cast<nfdfiltersize_t>(allowed_files.size()), default_path_value.get()
    );
    if (result == NFD_OKAY) {
        std::vector<std::filesystem::path> result_vector{};

        nfdpathsetsize_t count_paths{};
        const auto temp_result = NFD::PathSet::Count(out_paths, count_paths);
        ASSERT(temp_result == NFD_OKAY && "PathSet get count is successful");

        for (nfdpathsetsize_t i = 0; i < count_paths; ++i) {
            NFD::UniquePathSetPathU8 out_path{};
            const auto temp_result2 = NFD::PathSet::GetPath(out_paths, i, out_path);
            ASSERT(temp_result2 == NFD_OKAY && "PathSet get path is successful");
            result_vector.emplace_back(out_path.get());
        }

        return result_vector;
    }

    if (result == NFD_CANCEL) {
        return helper::unexpected<std::string>{ "The user pressed cancel." };
    }

    return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
}

[[nodiscard]] helper::expected<std::filesystem::path, std::string> helper::openFolderDialog(
        helper::optional<std::filesystem::path> default_path
) {

    NFD::UniquePathU8 out_path{};

    const auto path_deallocator = [](const nfdu8char_t* const char_value) {
        if (char_value == nullptr) {
            return;
        }

        delete[] char_value; // NOLINT(cppcoreguidelines-owning-memory)
    };


    UniquePtr<nfdu8char_t> default_path_value{ nullptr, path_deallocator };

    if (default_path.has_value()) {
        const auto& str = default_path.value().string();
        const auto str_size = str.size() + 1;
        default_path_value = UniquePtr<nfdu8char_t>{ new nfdu8char_t[str_size], path_deallocator };
        std::memcpy(default_path_value.get(), str.c_str(), str_size * sizeof(nfdu8char_t));
    }

    const nfdresult_t result = NFD::PickFolder(out_path, default_path_value.get());
    if (result == NFD_OKAY) {
        return std::filesystem::path{ out_path.get() };
    }

    if (result == NFD_CANCEL) {
        return helper::unexpected<std::string>{ "The user pressed cancel." };
    }

    return helper::unexpected<std::string>{ "Error: " + std::string{ NFD::GetError() } };
}


#endif
