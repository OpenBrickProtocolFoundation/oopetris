project(
    'oopetris',
    'cpp',
    license: 'MIT',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'c_std=c14',
        'cpp_std=c++14',
        'warning_level=3',
        'werror=true',
        'strip=true',
        'b_ndebug=if-release',
    ],
    version: '1.0.0-alpha',
)



deps = []


deps += dependency(
    'sdl2',
    required: true,
    fallback: 'sdl2',
)

inc_dirs = []
src_files = []

subdir('src')

# /home/totto/Code/vcpkg/packages/pkgconf_x64-linux
executable(
    'seatbelt',
    src_files,
    include_directories: [
        inc_dirs,
        include_directories('src'),
    ],
    dependencies: deps,
)


