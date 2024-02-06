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
    compile_args += ['/std:c++latest']
elif cpp.get_id() == 'gcc' and not meson.is_cross_build()
    compile_args += ['-std=c++23', '-Wold-style-cast']
elif cpp.get_id() == 'clang'
    compile_args += ['-std=c++2b']
else
    compile_args += ['-std=c++20']
endif

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
    compile_args += ['-DDEBUG_BUILD']
endif


deps = []
inc_dirs = []



native = true
only_allow_native_libs = false
if (meson.is_cross_build())
    if (host_machine.system() == 'android')

        native = false

    elif host_machine.system() == 'switch'

        native = false
        only_allow_native_libs = true
    endif
endif


sdl2_dep = dependency(
    'sdl2',
    'SDL2',
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
    'sdl2_ttf',
    'SDL2_ttf',
    allow_fallback: not only_allow_native_libs,
    required: true,
    native: native,
)


# a dirty thing atm, until mpg123 is ported to meson (maybe never...)
mpg123_dep = dependency(
    'mpg123',
    allow_fallback: true,
    required: false,
    native: native,
)
sdl2_mixer_flags = ['flac=enabled']
sdl2_mixer_defines = ['-DAUDIO_WITH_FLAC_SUPPORT']
if mpg123_dep.found()
    sdl2_mixer_flags += 'mpg123=enabled'
    sdl2_mixer_defines += '-DAUDIO_WITH_MP3_SUPPORT'
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
    'sdl2_mixer',
    'SDL2_mixer',
    allow_fallback: not only_allow_native_libs,
    required: true,
    native: native,
    default_options: sdl2_mixer_flags,
)


if (meson.is_cross_build() and host_machine.system() == 'switch')

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

if (meson.is_cross_build() and host_machine.system() == 'switch')

    switch_dependencies = [
        'EGL',
        'GLESv2',
        'glapi',
        'drm_nouveau',
        'nx',
        'freetype',
        'z',
        'bz2',
        'png',
        'opusfile',
        'opus',
        'vorbisfile',
        'vorbis',
        'ogg',
        'modplug',
        'harfbuzz',
        'FLAC',
    ]


    library_dirs = meson.get_external_property('library_dirs')

    foreach dep : switch_dependencies
        deps += cpp.find_library(
            dep,
            required: true,
            dirs: library_dirs,
        )
    endforeach



endif



src_files = []

subdir('src')

## TODO: only istall needed ones, since sometimes we only need e.g. flacs or mp3 and no icons etc.
## install assets
install_subdir('assets', install_dir: 'share/oopetris')

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

    elf_file = build_target(
        'oopetris.elf',
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
        target_type: 'executable',
    )


    use_nacp = ['true', 'True', '1', true].contains(
        meson.get_external_property('USE_NACP'),
    )

    elf2nro = find_program('elf2nro')
    # executable input elf file, output nro file
    NROFLAGS = [elf2nro, 'oopetris.elf', 'oopetris.nro']
    NRO_DEPS = [elf_file]
    NACP_DEPS = NRO_DEPS

    fs = import('fs')

    if use_nacp
        nacptool = find_program('nacptool')
        NACPFLAGS = [nacptool, '--create']

        APP_NAME = meson.get_external_property('APP_NAME', 'oopetris')
        NACPFLAGS += APP_NAME


        APP_AUTHOR = meson.get_external_property('APP_AUTHOR', '')
        if APP_AUTHOR == ''
            error('If USE_NACP is set, you have to provie an APP_AUTHOR')
        endif
        NACPFLAGS += APP_AUTHOR

        APP_VERSION = meson.get_external_property('APP_VERSION', '')
        if APP_VERSION == ''
            error('If USE_NACP is set, you have to provie an APP_VERSION')
        endif
        NACPFLAGS += APP_VERSION

        NACPFLAGS += 'oopetris.nacp' # outfile


        APP_TITLEID = meson.get_external_property('APP_TITLEID', '')

        # optional
        if APP_TITLEID != ''
            NACPFLAGS += '--titleid=' + APP_TITLEID
        endif


        # nacptool --create name author version oopetris.nacp 
        # optional: --titleid=<titleID>
        nacp_file = custom_target(
            'oopetris.nacp',
            command: NACPFLAGS,
            output: ['oopetirs.nacp'],
            depends: NACP_DEPS,
        )
        NROFLAGS += '--nacp=oopetris.nacp'
        NRO_DEPS += nacp_file


    endif


    default_icon = meson.get_external_property('libnx') / 'default_icon.jpg'

    app_icon = meson.get_external_property('APP_ICON', default_icon)

    if not fs.exists(app_icon)
        error('APP_ICON should exist, but doesn\'t: \'' + app_icon + '\'')
    endif

    if not fs.is_absolute(app_icon)
        app_icon = meson.project_source_root() / app_icon
    endif


    NROFLAGS += '--icon=' + app_icon


    romfs_dir = meson.get_external_property('romfs_dir')

    if not fs.exists(romfs_dir)
        error('romfs_dir should exist, but doesn\'t: \'' + romfs_dir + '\'')
    endif

    if not fs.is_absolute(romfs_dir)
        romfs_dir = meson.project_source_root() / romfs_dir
    endif


    NROFLAGS += '--romfsdir=' + romfs_dir

    # elf2nro oopetris.elf oopetris.nro NROFLAGS
    custom_target(
        'oopetris.nro',
        command: NROFLAGS,
        depends: NRO_DEPS,
        output: ['oopetris.nro'],
        build_by_default: true,
    )


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
