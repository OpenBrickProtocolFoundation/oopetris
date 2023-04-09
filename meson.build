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

sdl2_dep = dependency('sdl2', required: false)
if not sdl2_dep.found()
    deps += dependency('sdl2', required: true, fallback: 'sdl2')
else
    deps += sdl2_dep
endif

sdl2_ttf_dep = dependency('sdl2_ttf', required: false)
if not sdl2_ttf_dep.found()
    deps += dependency(
        'sdl2_ttf',
        required: true,
        fallback: 'sdl2_ttf',
    )
else
    deps += sdl2_ttf_dep
endif

spdlog_dep = dependency('spdlog', required: false)
if not spdlog_dep.found()
    deps += dependency(
        'spdlog',
        required: true,
        fallback: 'spdlog',
    )
else
    deps += spdlog_dep
endif

nlohmann_json_dep = dependency('nlohmann_json', required: false)
if not nlohmann_json_dep.found()
    deps += dependency(
        'nlohmann_json',
        required: true,
        fallback: 'nlohmann_json',
    )
else
    deps += nlohmann_json_dep
endif

deps += dependency('tl-optional', required: true, fallback: ['tl-optional'])
deps += dependency('tl-expected', required: true, fallback: ['tl-expected'])
deps += dependency('magic_enum', required: true, fallback: ['magic_enum'])
deps += dependency('argparse', required: true, fallback: ['argparse'])

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
