#include "./oopetris.hpp"


sol::table lua_integration::open_oopetris(sol::this_state L) {

    using namespace oopetris::types;

    sol::state_view lua(L);
    sol::table module = lua.create_table();
    module.new_usertype<test>("test", sol::constructors<test(), test(int)>(), "value", &test::value);

    return module;
}


#if defined(SOL2_LUA_SHARED_MODULE)

extern "C" OOPETRIS_API int luaopen_oopetris(lua_State* state) {
    return sol::stack::call_lua(L, 1, lua::open_oopetris);
}

#endif
