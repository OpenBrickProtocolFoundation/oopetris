project(
    'oopetris_c_wrapper',
    'cpp',
    'c',
    meson_version: '>=1.3.0',
    default_options: {
        'buildtype': 'debug',
        'optimization': '3',
        'strip': true,
        'cpp_std': ['c++23', 'c++latest', 'vc++latest', 'c++20'],
        'b_ndebug': 'if-release',

    },
    version: '0.5.6',
)

deps = []

deps += dependency('oopetris-recordings')

src_files = []

subdir('src')

src_files

liboopetris_c_wrapper = library(
    'oopetris_c_wrapper',
    src_files,
    dependencies: deps,
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    version: meson.project_version(),
    install: true,
)

liboopetris_c_wrapper_dep = declare_dependency(
    link_with: liboopetris_c_wrapper,
    dependencies: deps,
    version: meson.project_version(),
)
meson.override_dependency('liboopetris-c-wrapper', liboopetris_c_wrapper_dep)

# generate pkgconfig files
pkg = import('pkgconfig')
pkg.generate(
    liboopetris_c_wrapper,
    description: 'The C Wrapper library for oopetris',
    name: 'oopetris-c-wrapper',
    filebase: 'oopetris-c-wrapper',
    subdirs: 'oopetris',
)

if get_option('tests')
    subdir('tests')
endif
