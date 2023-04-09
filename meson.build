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

if meson.version().version_compare('<0.60.0')
    error(
        'meson version must be higher than 0.60.0 to support some used features',
    )
endif


cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
    add_project_arguments('/std:c++latest', language: 'cpp')
elif cpp.get_id() == 'gcc'
    add_project_arguments(
        '-std=c++23',
        language: 'cpp',
    )

    if host_machine.system() == 'windows'
        add_project_arguments(
            '-Wno-shadow',
            '-Wno-complain-wrong-lang',
            language: 'cpp',
        )

        add_global_arguments('-Wno-shadow', language: 'cpp')
    endif

elif cpp.get_id() == 'clang'
    add_project_arguments('-std=c++2b', language: 'cpp')
else
    add_project_arguments('-std=c++20', language: 'cpp')
endif

deps = []

if cpp.get_id() == 'gcc' and host_machine.system() == 'windows'
    deps += dependency(
        'sdl2',
        'SDL2',
        required: true,
        fallback: 'sdl2',
        default_options: [
            'buildtype=release',
            'optimization=3',
            'strip=true',
            'b_ndebug=if-release',
            'cpp_args=-Wno-shadow',
        ],
    )
else
    deps += dependency('sdl2', 'SDL2', required: true, fallback: 'sdl2')
endif

deps += dependency('sdl2_ttf', 'SDL2_ttf', required: true, fallback: 'sdl2_ttf')
deps += dependency('spdlog', required: true, fallback: 'spdlog')
deps += dependency('nlohmann_json', required: true, fallback: 'nlohmann_json')

deps += dependency('tl-optional', required: true, fallback: ['tl-optional'])
deps += dependency('magic_enum', required: true, fallback: ['magic_enum'])

inc_dirs = []
src_files = []

subdir('src')

executable(
    'oopetris',
    src_files,
    include_directories: inc_dirs,
    dependencies: deps,
    cpp_args: [
        
    ],
    override_options: [
        'warning_level=3',
        'werror=true',
    ],
)
