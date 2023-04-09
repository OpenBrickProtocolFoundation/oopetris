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

compile_args = []

cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
    add_project_arguments('/std:c++latest', language: 'cpp')
elif cpp.get_id() == 'gcc'
    add_project_arguments(
        '-std=c++23',
        language: 'cpp',
    )

    if host_machine.system() == 'windows'
        compile_args += ['-Wno-shadow', '-Wno-complain-wrong-lang']
        add_project_arguments(
            '-Wno-complain-wrong-lang',
            language: 'cpp',
        )
    endif

elif cpp.get_id() == 'clang'
    add_project_arguments('-std=c++2b', language: 'cpp')
else
    add_project_arguments('-std=c++20', language: 'cpp')
endif

deps = []

native = true
if meson.is_cross_build()
    native = false
endif



sdl2_dep = dependency('sdl2', required: false, native: native)
if not sdl2_dep.found()
    deps += dependency('sdl2', required: true, fallback: 'sdl2', native: native)
else
    deps += sdl2_dep
endif

sdl2_ttf_dep = dependency('sdl2_ttf', required: false, native: native)
if not sdl2_ttf_dep.found()
    deps += dependency(
        'sdl2_ttf',
        required: true,
        fallback: 'sdl2_ttf',
        native: native,
    )
else
    deps += sdl2_ttf_dep
endif

spdlog_dep = dependency('spdlog', required: false, native: native)
if not spdlog_dep.found()
    deps += dependency(
        'spdlog',
        required: true,
        fallback: 'spdlog',
        native: native,
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
        native: native,
    )
else
    deps += nlohmann_json_dep
endif

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


inc_dirs = []
src_files = []

subdir('src')

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
