lua_src_files = files('oopetris.cpp', 'oopetris.hpp', 'oopetris_api.hpp')

lua_inc_dirs = include_directories('.')


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
    install:true,
    install_dir:''
)
