#include "files.hpp"


std::filesystem::path utils::get_files_dir() {
    return std::filesystem::path{ "." };
}
