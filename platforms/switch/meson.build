
# libraries

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



## compilation

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
NROFLAGS = [elf2nro, elf_file.full_path(), 'oopetris.nro']
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

if not fs.is_absolute(app_icon)
    app_icon = meson.project_source_root() / app_icon
endif


if not fs.exists(app_icon)
    error('APP_ICON should exist, but doesn\'t: \'' + app_icon + '\'')
endif



NROFLAGS += '--icon=' + app_icon


romfs_dir = meson.get_external_property('romfs_dir')

if not fs.is_absolute(romfs_dir)
    romfs_dir = meson.project_source_root() / romfs_dir
endif

if not fs.exists(romfs_dir)
    error('romfs_dir should exist, but doesn\'t: \'' + romfs_dir + '\'')
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
