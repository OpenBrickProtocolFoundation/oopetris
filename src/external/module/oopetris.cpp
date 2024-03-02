#include "./oopetris.hpp"

#include "sol_include.hpp"

namespace oopetris {
    sol::table open_oopetris(sol::this_state L) {
        sol::state_view lua(L);
        sol::table module = lua.create_table();
        module.new_usertype<test>("test", sol::constructors<test(), test(int)>(), "value", &test::value);

        return module;
    }

} // namespace oopetris

extern "C" int luaopen_oopetris(lua_State* L) {
    // pass the lua_State,
    // the index to start grabbing arguments from,
    // and the function itself
    // optionally, you can pass extra arguments to the function
    // if that's necessary, but that's advanced usage and is
    // generally reserved for internals only
    return sol::stack::call_lua(L, 1, oopetris::open_oopetris);
}
