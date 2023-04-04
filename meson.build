project(
    'oopetris',
    'cpp',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'strip=true',
        'b_ndebug=if-release',
    ],
    version: '1.0.0-alpha',
)

cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
    add_project_arguments('/std:c++latest', language: 'cpp')
elif cpp.get_id() == 'gcc'
    add_project_arguments('-std=c++23', language: 'cpp')
else
    add_project_arguments('-std=c++20', language: 'cpp')
endif

subdir('tools')

deps = []

# if only meson would support setting this for the pkg-config inside dependency()
#env = environment()
#env.append('PKG_CONFIG_PATH','./tools/vcpkg_installed/x64-linux-release/lib/pkgconfig') 

deps += dependency('SDL2', required: true)
deps += dependency('tl-optional', required: true) ## this is a header only lib!
deps += dependency('SDL2_ttf', required: true)


inc_dirs = []
src_files = []

subdir('src')

executable(
    'oopetris',
    src_files,
    include_directories: inc_dirs,
    link_with: vcpkg_link_with,
    dependencies: deps,
    override_options: [
        'warning_level=3',
        'werror=true',
    ],
)
