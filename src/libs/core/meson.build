core_src_files = []
core_header_files = [files('core.hpp')]

core_include_dir = include_dir / 'oopetris' / 'core'

subdir('game')
subdir('hash-library')
subdir('helper')

temp = core_lib.get('compile_args')
temp += '-DOOPETRIS_LIBRARY_CORE_TYPE=' + (get_option('default_library') == 'static' ? '1' : '0')
core_lib += {
    'compile_args': temp,
}
temp = 0

liboopetris_core = library(
    'oopetris_core',
    core_src_files,
    core_header_files,
    include_directories: core_lib.get('inc_dirs'),
    dependencies: core_lib.get('deps'),
    cpp_args: core_lib.get('compile_args'),
    cpp_shared_args: ['-DOOPETRIS_LIBRARY_CORE_EXPORT'],
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    version: meson.project_version(),
    install: true,
)
## remove '-stdlib=libc++' from the list, if it's present
core_dep_compile_args = []
foreach value : core_lib.get('compile_args')
    if value != '-stdlib=libc++'
        core_dep_compile_args += value
    endif
endforeach

liboopetris_core_dep = declare_dependency(
    link_with: liboopetris_core,
    include_directories: core_lib.get('inc_dirs'),
    compile_args: core_dep_compile_args,
    dependencies: core_lib.get('deps'),
    version: meson.project_version(),
)
meson.override_dependency('liboopetris-core', liboopetris_core_dep)

install_headers(
    files('core.hpp'),
    install_dir: core_include_dir,
    preserve_path: true,
)

# generate pkgconfig files
pkg = import('pkgconfig')

pkg.generate(
    liboopetris_core,
    description: 'The core library for oopetris',
    name: 'oopetris-core',
    filebase: 'oopetris-core',
    subdirs: 'oopetris',
    extra_cflags: core_dep_compile_args,
    variables: ['compiler=' + pkg_cpp_compiler, 'cpp_stdlib=' + pkg_cpp_stdlib],
)

# setting this to strings, so += {...} gets detected as an error, if it is done after that
core_lib = 'undefined'
