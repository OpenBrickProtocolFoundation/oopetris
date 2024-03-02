lua_src_files = files('oopetris.cpp', 'oopetris.hpp')

lua_inc_dirs = include_directories('.')

# in debug mode auto => No, since that makes debugging easier, otherwise yes

build_shared_lua_module = get_option('sol2_build_shared_lib').disable_auto_if(is_debug).allowed()


if build_shared_lua_module

    compile_args += 'SOL2_LUA_SHARED_MODULE'

    library(
        'oopetris',
        lua_src_files,
        include_directories: lua_inc_dirs,
        dependencies: lua_deps,
        cpp_args: compile_args,
        override_options: {
            'warning_level': '3',
            'werror': true,
        },
        install: true,
        install_dir: datadir / 'lua' / lua_version,
    )

else

    src_files += lua_src_files

endif
