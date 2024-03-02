#pragma once

#include "./oopetris_api.hpp"

// forward declare as a C struct
// so a pointer to lua_State can be part of a signature
extern "C" {
struct lua_State;
}
// you can replace the above if you're fine with including
// <sol.hpp> earlier than absolutely necessary

namespace oopetris {

    struct test {
        int value;

        test() = default;
        test(int val) : value(val) { }
    };

} // namespace oopetris

// this function needs to be exported from your
// dll. "extern 'C'" should do the trick, but
// we're including platform-specific stuff here to help
// see my_object_api.hpp for details
extern "C" OOPETRIS_API int luaopen_oopetri(lua_State* L);
