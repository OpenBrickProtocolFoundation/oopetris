project(
    'oopetris-backend',
    'cpp',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'strip=true',
        'cpp_std=c++20',
        'b_ndebug=if-release',
    ],
)


deps = []
src_files = []
inc_dirs = []

deps += dependency(
    'oatpp',
    version: '1.3.0',
    default_options: ['tests=disabled'],
)
deps += dependency(
    'oatpp-websocket',
    version: '1.3.0',
    default_options: ['tests=false'],
)
deps += dependency(
    'oatpp-zlib',
    version: '1.3.0',
    default_options: ['tests=false'],
)
deps += dependency(
    'oatpp-sqlite',
    version: '1.3.0',
    default_options: ['tests=false'],
)
deps += dependency(
    'oatpp-swagger',
    version: '1.3.0',
    default_options: ['tests=false'],
)

deps += dependency(
    'stduuid',
    version: '1.2.3',
    default_options: ['tests=false'],
)


HOSTNAME = 'oopetris.totto.tk'
PROTOCOL = 'https'

# TODO: add real keys, this has to be done using nginx and certbot or similar things
flags = [
    '-DDATABASE_FILE="' + (meson.project_source_root() / 'db.sqlite') + '"',
    '-DDATABASE_MIGRATIONS="' + (meson.project_source_root() / 'sql') + '"',
    '-DLISTEN_PORT=6767',
    '-DROOT_PATH="' + (meson.project_source_root() / 'frontend') + '"',
    '-DHOSTNAME="' + HOSTNAME + '"',
    '-DSWAGGER_SERVER_URL="' + PROTOCOL + '://' + HOSTNAME + '"',
]



subdir('src')


executable(
    'oopetris_backend',
    src_files,
    include_directories: inc_dirs,
    cpp_args: flags,
    dependencies: deps,
    override_options: ['warning_level=3', 'werror=false'],
)
