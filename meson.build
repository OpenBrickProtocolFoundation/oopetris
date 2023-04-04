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

deps = []

deps += dependency('sdl2', required: true, fallback: ['libsdl2', 'sdl2_dep'])
# add this later: deps += dependency('tl-optional', required: true) ## this is a header only lib!
deps += dependency(
    'sdl2_ttf',
    required: true,
    fallback: ['libsdl2_ttf', 'sdl2_ttf_dep'],
)

deps += dependency('tl-optional', required: true, fallback: ['tl-optional'])

inc_dirs = []
src_files = []

subdir('src')

executable(
    'oopetris',
    src_files,
    include_directories: inc_dirs,
    dependencies: deps,
    override_options: [
        'warning_level=3',
        'werror=true',
    ],
)
