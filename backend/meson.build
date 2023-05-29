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
    'oatpp-openssl',
    version: '1.3.0',
    default_options: ['tests=false'],
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
# deps += dependency(
#     'oatpp-postgresql',
#     version: '1.3.0',
#     default_options: ['tests=false'],
# )

# TODO: add real keys, this has to be done using nginx and certbot or similar things
flags = [
    '-DCERT_PEM_PATH="' + (
        meson.project_source_root() / 'cert' / 'test_key.pem'
    ) + '"',
    '-DCERT_CRT_PATH="' + (
        meson.project_source_root() / 'cert' / 'test_cert.crt'
    ) + '"',
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
