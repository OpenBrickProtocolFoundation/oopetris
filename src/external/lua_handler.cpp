
#include "lua_handler.hpp"

#include "module/oopetris.hpp"


#include <fmt/format.h>
#include <fstream>
#include <iostream>

helper::expected<external::LUAHandler, std::string> external::load_from_file(const std::filesystem::path& file) {

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

    std::vector<sol::lib> libraries{ sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math,
                                     sol::lib::table };


    // TODO: check if using luajit
    libraries.push_back(sol::lib::jit);
#ifdef DEBUG_BUILD
    libraries.push_back(sol::lib::debug);
#endif

    sol::state lua{};

    if (use_typed_lua) {
        libraries.push_back(sol::lib::io);
        libraries.push_back(sol::lib::os);
    }

    for (const auto lib : libraries) {
        lua.open_libraries(lib);
    }

    if (use_typed_lua) {
        //TODO: generate lua file from tl file, after checking types
        lua.safe_script(
                R"--(
local tl = require("tl")
tl.loader())--"
        );
    }

#if !defined(SOL2_LUA_SHARED_MODULE)


    lua.require("oopetris", sol::c_call<decltype(&lua_integration::open_oopetris), &lua_integration::open_oopetris>);

#endif


    //TODO: use thread, environment etc.

    try {

        const auto result = lua.safe_script_file(file.string());

    } catch (const sol::error& e) {
        std::cout << "an expected error has occurred: " << e.what() << "\n";
    }

    return external::LUAHandler{};

    //
}
