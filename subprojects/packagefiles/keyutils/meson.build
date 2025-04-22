project(
    'keyutils',
    'c',
    meson_version: '>=1.3.0',
    version: '1.6.3',
    license: 'GPL-2.0',
    default_options: {
        'optimization': '3',
        'warning_level': '2',
        'werror': true,
        'strip': true,
        'cpp_std': ['c++23', 'c++latest', 'vc++latest', 'c++20'],
        'b_ndebug': 'if-release',
    },
)

inc_dirs = include_directories('.')

src_files = files('keyutils.c')

API_VERSON = '1.10'

sh = find_program('sh')

current_date = run_command(
    sh,
    '-c',
    'date -u +%F',
    check: true,
).stdout().strip()

compilation_flags = [
    '-DPKGBUILD="' + current_date + '"',
    '-DPKGVERSION="keyutils-' + meson.project_version() + '"',
    '-DAPIVERSION="libkeyutils-' + API_VERSON + '"',
]

if get_option('no_glibc_keyerr')
    compilation_flags += '-DNO_GLIBC_KEYERR'
endif

keyutils_lib = library(
    'keyutils',
    src_files,
    include_directories: inc_dirs,
    install: true,
    c_args: compilation_flags,
)

keyutils_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
    link_with: keyutils_lib,
)

meson.override_dependency('keyutils', keyutils_dep)
