core_src_files = []
core_header_files = [files('core.hpp')]

core_include_dir = include_dir / 'oopetris' / 'core'

subdir('game')
subdir('hash-library')
subdir('helper')

liboopetris_core = library(
    'oopetris_core',
    core_src_files,
    core_header_files,
    include_directories: core_lib.get('inc_dirs'),
    dependencies: core_lib.get('deps'),
    cpp_args: core_lib.get('compile_args'),
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    version: meson.project_version(),
    install: true,
)

liboopetris_core_dep = declare_dependency(
    link_with: liboopetris_core,
    include_directories: core_lib.get('inc_dirs'),
    compile_args: core_lib.get('compile_args'),
    dependencies: core_lib.get('deps'),
    version: meson.project_version(),
)
meson.override_dependency('liboopetris-core', liboopetris_core_dep)

# setting this to strings, so += {...} gets detected as an error, if it is done after that
core_lib = 'undefined'

install_headers(
    files('core.hpp'),
    install_dir: core_include_dir,
    preserve_path: true,
)

# generate pkgconfig files
pkg = import('pkgconfig')
pkg.generate(
    liboopetris_core,
    name: 'oopetris-core',
    filebase: 'oopetris-core',
)
