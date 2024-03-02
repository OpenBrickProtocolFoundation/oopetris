
#include "load_file.hpp"

#include <fmt/format.h>
#include <fstream>
#include <iostream>

helper::expected<bool, std::string> external::load_file(const std::filesystem::path& file) {

    if (not std::filesystem::exists(file)) {
        return helper::unexpected<std::string>{ "File does not exist" };
    }

    const auto teal_extension = ".tl";
    const auto lua_extension = ".lua";

    const auto file_extension = file.extension().string();

    bool use_typed_lua{};
    if (file_extension == teal_extension) {
        use_typed_lua = true;
    } else if (file_extension == lua_extension) {
        use_typed_lua = false;
    } else {
        return helper::unexpected<std::string>{ fmt::format(
                "Not a valid file: only supports file with extension: {}, {} but got {}", teal_extension, lua_extension,
                file_extension
        ) };
    }

    sol::state lua{};
    lua.open_libraries(sol::lib::package, sol::lib::base);

    if (use_typed_lua) {
        lua.safe_script(
                R"--(local tl = require("tl")
tl.loader())--"
        );
    }


    //TODO: use thread, environment etc.

    try {

        const auto result = lua.safe_script_file(file.string());
    } catch (const sol::error& e) {
        std::cout << "an expected error has occurred: " << e.what() << "\n";
    }

    return true;

    //
}
