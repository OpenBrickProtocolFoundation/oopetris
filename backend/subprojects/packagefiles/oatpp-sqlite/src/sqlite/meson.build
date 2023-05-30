sqlite_amalgamation_version = '3.33.0'
sqlite_amalgamation_so_version = sqlite_amalgamation_version.split('.').get(0)

lib_sqlite_amalgamation = library(
    'sqlite_amalgamation',
    files('sqlite3.c', 'sqlite3.h'),
    include_directories: include_directories('.'),
    install: get_option('install'),
    c_args: flags,
    soversion: sqlite_amalgamation_so_version,
    version: sqlite_amalgamation_version,
)


sqlite_amalgamation_dep = declare_dependency(
    include_directories: include_directories('.'),
    link_with: lib_sqlite_amalgamation,
    version: sqlite_amalgamation_version,
)
