project(
    'oopetris',
    'cpp',
    license: 'MIT',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'cpp_std=c++14',
        'warning_level=0',
        'werror=false',
        'strip=true',
        'b_ndebug=if-release',
    ],
    version: '1.0.0-alpha',
)


# msvc can handle c++14 and constexpr better (due to some NON standard conform fixes, but gcc can't, it needs c++17 for that!)
cpp = meson.get_compiler('cpp')
if cpp.get_id() != 'msvc'
    add_project_arguments('-std=c++17', language: 'cpp')
endif

deps = []


sdl2_dep = dependency('sdl2', required: false)
if not sdl2_dep.found()
    deps += dependency(
        'sdl2',
        required: true,
        fallback: 'sdl2',
    )
    # to support both "SDL2/..." and "..." includes
    add_project_arguments('-D__SDL2__FOLDER_MISSING__', language: 'cpp')
else
    deps += sdl2_dep
endif

inc_dirs = []
src_files = []

subdir('src')

executable(
    'oopetris',
    src_files,
    include_directories: inc_dirs,
    dependencies: deps,
)
