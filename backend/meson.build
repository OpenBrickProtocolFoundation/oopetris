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
deps += dependency('tl-optional')

# TODO: use this
#deps += dependency('HTTPRequest')

HOSTNAME = 'oopetris.totto.tk'
PROTOCOL = 'https'

if get_option('CLIENT_ID') == '' or get_option('CLIENT_SECRET') == ''
    error('CLIENT_ID and CLIENT_SECRET are required')
endif

# TODO: add real keys, this has to be done using nginx and certbot or similar things
flags = [
    '-DDATABASE_FILE="' + (meson.project_source_root() / 'db.sqlite') + '"',
    '-DDATABASE_MIGRATIONS="' + (meson.project_source_root() / 'sql') + '"',
    '-DLISTEN_PORT=6767',
    '-DROOT_PATH="' + (meson.project_source_root() / 'frontend') + '"',
    '-DHOSTNAME="' + HOSTNAME + '"',
    '-DSERVER_URL="' + PROTOCOL + '://' + HOSTNAME + '"',
    '-DTWITCH_CLIENT_ID="' + get_option('CLIENT_ID') + '"',
    '-DTWITCH_CLIENT_SECRET="' + get_option('CLIENT_SECRET') + '"',
]



subdir('src')
src_files += files('external/HTTPRequest.hpp')
inc_dirs += include_directories('external')

executable(
    'oopetris_backend',
    src_files,
    include_directories: inc_dirs,
    cpp_args: flags,
    dependencies: deps,
    override_options: ['warning_level=3', 'werror=false'],
)
