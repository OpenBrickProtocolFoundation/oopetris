recordings_src_files = []
recordings_header_files = [files('recordings.hpp')]

recordings_include_dir = include_dir / 'oopetris' / 'recordings'

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
    cpp_shared_args: ['-DOOPETRIS_LIBRARY_EXPORT'],
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    version: meson.project_version(),
    install: true,
)

## remove '-stdlib=libc++' from the list, if it's present
recordings_dep_compile_args = []
foreach value : recordings_lib.get('compile_args')
    if value != '-stdlib=libc++'
        recordings_dep_compile_args += value
    endif
endforeach

liboopetris_recordings_dep = declare_dependency(
    link_with: liboopetris_recordings,
    include_directories: recordings_lib.get('inc_dirs'),
    compile_args: recordings_dep_compile_args,
    dependencies: recordings_lib.get('deps'),
    version: meson.project_version(),
)
meson.override_dependency('liboopetris-recordings', liboopetris_recordings_dep)

install_headers(
    files('recordings.hpp'),
    install_dir: recordings_include_dir,
    preserve_path: true,
)

# generate pkgconfig files
pkg = import('pkgconfig')
pkg.generate(
    liboopetris_recordings,
    description: 'The recordings library for oopetris',
    name: 'oopetris-recordings',
    filebase: 'oopetris-recordings',
    subdirs: 'oopetris',
    extra_cflags: recordings_dep_compile_args,
    variables: ['compiler=' + pkg_cpp_compiler, 'cpp_stdlib=' + pkg_cpp_stdlib],
    requires: ['oopetris-core', 'fmt'],
)

# setting this to strings, so += {...} gets detected as an error, if it is done after that
recordings_lib = 'undefined'
