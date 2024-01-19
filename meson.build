project(
    'oopetris',
    'c',
    'cpp',
    meson_version: '>=0.60.0',
    default_options: [
        'buildtype=release',
        'optimization=3',
        'strip=true',
        'cpp_std=c++20',
        'b_ndebug=if-release',
    ],
    version: '1.0.0-alpha',
)


compile_args = []

cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
    add_project_arguments('/std:c++latest', language: 'cpp')
elif cpp.get_id() == 'gcc' and not meson.is_cross_build()
    add_project_arguments('-std=c++23', '-Wold-style-cast', language: 'cpp')
elif cpp.get_id() == 'clang'
    add_project_arguments('-std=c++2b', language: 'cpp')
else
    add_project_arguments('-std=c++20', language: 'cpp')
endif

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    add_project_arguments('-DDEBUG_BUILD', language: 'cpp')
endif


deps = []
inc_dirs = []


native = true
only_allow_native_libs = false
sdl_name = 'sdl2'
if (meson.is_cross_build())
    if (host_machine.system() == 'android')

        native = false

    elif host_machine.system() == 'switch'

        native = false
        only_allow_native_libs = true

    elif host_machine.system() == '3ds'

        native = false
        only_allow_native_libs = true
        sdl_name = 'sdl'
        compile_args += ['-D_USE_SDL_LEGACY_VERSION']
    endif
endif




sdl2_dep = dependency(
    sdl_name,
    sdl_name.to_upper(),
    allow_fallback: false,
    required: only_allow_native_libs,
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
        default_options: ['test=false'],
    )
    deps += dependency(
        'sdl2main',
        required: true,
        fallback: 'sdl2',
        native: native,
    )
endif




deps += dependency(
    sdl_name + '_ttf',
    sdl_name.to_upper() + '_ttf',
    allow_fallback: not only_allow_native_libs,
    required: true,
    native: native,
)


if sdl_name == 'sdl'
    deps += dependency(
        sdl_name + '_gfx',
        sdl_name.to_upper() + '_gfx',
        allow_fallback: not only_allow_native_libs,
        required: true,
        native: native,
    )

    compile_args += ['-D_HAVE_SDL_GFX']
endif


# a dirty thing atm, until mpg123 is ported to meson (maybe never...)
mpg123_dep = dependency(
    'mpg123',
    'libmpg123',
    allow_fallback: true,
    required: false,
    native: native,
)
sdl2_mixer_flags = ['flac=enabled']
sdl2_mixer_defines = ['-DAUDIO_WITH_FLAC_SUPPORT']
if mpg123_dep.found()
    sdl2_mixer_flags += 'mpg123=enabled'
    sdl2_mixer_defines += '-DAUDIO_WITH_MP3_SUPPORT'
    deps += mpg123_dep
else
    cpp = meson.get_compiler('cpp')
    mpg123_dep = cpp.find_library('mpg123', required: only_allow_native_libs)
    if mpg123_dep.found()
        sdl2_mixer_flags += 'mpg123=enabled'
        sdl2_mixer_defines += '-DAUDIO_WITH_MP3_SUPPORT'

        meson.override_dependency(
            'mpg123',
            mpg123_dep,
            native: native,
        )
    endif
endif



deps += dependency(
    sdl_name + '_mixer',
    sdl_name.to_upper() + '_mixer',
    allow_fallback: not only_allow_native_libs,
    required: true,
    native: native,
    default_options: sdl2_mixer_flags,
)


if (
    meson.is_cross_build()
    and (host_machine.system() == 'switch'
    or host_machine.system() == '3ds')
)

    fmt_lib = dependency(
        'fmt',
        required: false,
        native: native,
        allow_fallback: false,
    )

    if fmt_lib.found()
        deps += fmt_lib
    else

        fmt_header_only_dep = dependency(
            'fmt_header_only',
            required: true,
            native: native,
            allow_fallback: true,
            default_options: ['default_library=static'],
        )
        deps += fmt_header_only_dep

    endif


else

    deps += dependency(
        'fmt',
        required: true,
        native: native,
    )
endif

deps += dependency(
    'spdlog',
    required: true,
    native: native,
    default_options: ['tests=false'],
)

deps += dependency(
    'nlohmann_json',
    required: true,
    native: native,
)

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
    default_options: ['test=false'],
)
deps += dependency(
    'argparse',
    required: true,
    fallback: ['argparse'],
    native: native,
)

src_files = []

subdir('src')

## TODO: only istall needed ones, since sometimes we only need e.g. flacs or mp3 and no icons etc.
## install assets
install_subdir('assets', install_dir: 'share/oopetris')

exe_name = 'oopetris'

if meson.is_cross_build() and host_machine.system() == 'android'

    library(
        'oopetris',
        src_files,
        include_directories: inc_dirs,
        dependencies: deps,
        c_args: sdl2_mixer_defines,
        cpp_args: compile_args + sdl2_mixer_defines,
        override_options: ['warning_level=3', 'werror=true'],
        native: native,
    )
elif meson.is_cross_build() and host_machine.system() == 'switch'
    subdir('platforms/switch')
elif meson.is_cross_build() and host_machine.system() == '3ds'
    subdir('platforms/3ds')
else
    executable(
        'oopetris',
        src_files,
        include_directories: inc_dirs,
        dependencies: deps,
        c_args: sdl2_mixer_defines,
        cpp_args: compile_args + sdl2_mixer_defines,
        override_options: [
            'warning_level=3',
            'werror=true',
        ],
        native: native,
    )
endif
