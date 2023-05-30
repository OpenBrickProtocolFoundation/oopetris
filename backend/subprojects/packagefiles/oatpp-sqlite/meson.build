
project(
    'oatpp-sqlite',
    'cpp',
    'c', # for eventual SQLite amalgamation
    default_options: ['default_library=static', 'cpp_std=c++11'],
    version: '1.3.0',
    license: 'Apache-2.0',
)

oatpp_module_version = meson.project_version()
oatpp_module_so_version = (
    oatpp_module_version.split('.').get(0) + '.' + oatpp_module_version.split(
        '.',
    ).get(1)
)

flags = []


cpp = meson.get_compiler('cpp')
is_msvc_or_similar = cpp.get_id() == 'msvc' or cpp.get_id() == 'clang-cl'

if is_msvc_or_similar
    if get_option('msvc_link_static_runtime')
        message('MSVC: using statically-linked runtime (/MT and /MTd).')
        flags += '/MT'
    else
        message('MSVC: using dynamically-linked runtime (/MD and /MDd).')
        flags += '/MD'
    endif
endif


deps = []
src_files = []
inc_dirs = []


deps += dependency('oatpp', version: '1.3.0', default_options: ['tests=false'])
get_option('sqlite_amalgamation')

# three possible states are represented here, if sqlite_amalgamation is auto, it's used as fallback, if' its enabled, it is enforced, if it's disabled it is forbidden
if get_option('sqlite_amalgamation').enabled()
    subdir('src/sqlite')
    deps += sqlite_amalgamation_dep
else

    sqlite3_wrap_required = get_option('sqlite_amalgamation').disabled()
    sqlite3_dep = dependency('sqlite3', required: sqlite3_wrap_required)
    if sqlite3_dep.found()
        deps += sqlite3_dep
    else
        subdir('src/sqlite')
        deps += sqlite_amalgamation_dep
    endif
endif




subdir('src')


# TODO: install isn't well tought out atm
lib_oatpp_sqlite = library(
    'oatpp-sqlite',
    src_files,
    dependencies: deps,
    include_directories: inc_dirs,
    cpp_args: flags,
    install: get_option('install'),
    soversion: oatpp_module_so_version,
    version: oatpp_module_version,
)


oatpp_sqlite_dep = declare_dependency(
    include_directories: inc_dirs,
    link_with: lib_oatpp_sqlite,
    version: oatpp_module_version,
)


if get_option('tests')
    subdir('test')
endif

