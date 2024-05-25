recordings_src_files = []
recordings_header_files = []

subdir('utility')

recordings_lib += {
    'deps': [recordings_lib.get('deps'), liboopetris_core_dep],
    'inc_dirs': [recordings_lib.get('inc_dirs'), include_directories('.')],
}

liboopetris_recordings = library(
    'oopetris_recordings',
    recordings_src_files,
    recordings_header_files,
    include_directories: recordings_lib.get('inc_dirs'),
    dependencies: recordings_lib.get('deps'),
    cpp_args: recordings_lib.get('compile_args'),
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    version: meson.project_version(),
    install: true,
)

liboopetris_recordings_dep = declare_dependency(
    link_with: liboopetris_recordings,
    include_directories: recordings_lib.get('inc_dirs'),
    compile_args: recordings_lib.get('compile_args'),
    dependencies: recordings_lib.get('deps'),
    version: meson.project_version(),
)
meson.override_dependency('liboopetris-recordings', liboopetris_recordings_dep)

# setting this to strings, so += {...} gets detected as an error, if it is done after that
recordings_lib = 'undefined'

install_headers(
    recordings_header_files,
    subdir: 'oopetris/recordings',
    preserve_path: true,
)

# generate pkgconfig files
pkg = import('pkgconfig')
pkg.generate(
    liboopetris_recordings,
    name: 'oopetris-recordings',
    filebase: 'oopetris-recordings',
)
