project(
    'oopetris',
    'c',
    'cpp',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'strip=true',
        'cpp_std=c++20',
        'b_ndebug=if-release',
    ],
    version: '1.0.0-alpha',
)

if meson.version().version_compare('<0.60.0')
    error(
        'meson version must be higher than 0.60.0 to support some used features',
    )
endif

compile_args = []

cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
    add_project_arguments('/std:c++latest', language: 'cpp')
elif cpp.get_id() == 'gcc'
    add_project_arguments('-std=c++23', '-Wold-style-cast', language: 'cpp')
elif cpp.get_id() == 'clang'
    add_project_arguments('-std=c++2b', language: 'cpp')
else
    add_project_arguments('-std=c++20', language: 'cpp')
endif

deps = []
inc_dirs = []

native = true
if meson.is_cross_build() and host_machine.system() == 'android'
    native = false
endif


sdl2_dep = dependency(
    'sdl2',
    allow_fallback: false,
    required: false,
    native: native,
)

if sdl2_dep.found()
    deps += sdl2_dep
else
    deps += dependency(
        'sdl2',
        required: true,
        fallback: 'sdl2',
        native: native,
    )
    deps += dependency(
        'sdl2main',
        required: true,
        fallback: 'sdl2',
        native: native,
    )
endif

deps += dependency('sdl2_ttf', 'SDL2_ttf', required: true, native: native)
deps += dependency('sdl2_net', 'SDL2_net', required: true, native: native)
deps += dependency('spdlog', required: true, native: native)
deps += dependency('nlohmann_json', required: true, native: native)
deps += dependency(
    'tl-optional',
    required: true,
    fallback: ['tl-optional'],
    native: native,
)
deps += dependency(
    'tl-expected',
    required: true,
    fallback: ['tl-expected'],
    native: native,
)
deps += dependency(
    'magic_enum',
    required: true,
    fallback: ['magic_enum'],
    native: native,
)


deps += dependency(
    'argparse',
    required: true,
    fallback: ['argparse'],
    native: native,
)


src_files = []

subdir('src')

if meson.is_cross_build() and host_machine.system() == 'android'

    library(
        'oopetris',
        src_files,
        include_directories: inc_dirs,
        dependencies: deps,
        cpp_args: compile_args,
        override_options: ['warning_level=3', 'werror=true'],
        native: native,
    )

else
    executable(
        'oopetris',
        src_files,
        include_directories: inc_dirs,
        dependencies: deps,
        cpp_args: compile_args,
        override_options: [
            'warning_level=3',
            'werror=true',
        ],
        native: native,
    )
endif
