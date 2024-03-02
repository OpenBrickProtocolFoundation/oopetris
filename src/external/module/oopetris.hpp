#pragma once

#include "../sol_include.hpp"

// forward declare as a C struct
// so a pointer to lua_State can be part of a signature
extern "C" {
struct lua_State;
}
// you can replace the above if you're fine with including
// <sol.hpp> earlier than absolutely necessary

namespace oopetris {

    namespace types {

        struct test {
            int value;

            test() = default;
            test(int val) : value(val) { }
        };

    } // namespace types

} // namespace oopetris


#if defined(SOL2_LUA_SHARED_MODULE)

#if defined(_MSC_VER)
#define OOPETRIS_API __declspec(dllexport)
#else
#define OOPETRIS_API __attribute__((visibility("default")))
#endif


extern "C" OOPETRIS_API int luaopen_oopetris(lua_State* state);

#endif
namespace lua_integration {

    sol::table open_oopetris(sol::this_state L);

}


