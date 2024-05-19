## get the options object and "unpack" it

exe_name = nintendo_options[0]
src_files = nintendo_options[1]
deps = nintendo_options[2]

# libraries

dependencies = [
    'harfbuzz',
    'egl',
    'flac',
    'freetype2',
    'glapi',
    'glesv2',
    'ogg',
    'opus',
    'opusfile',
    'vorbis',
    'vorbisfile',
    'zlib',
]

dependencies_native = [
    'bz2',
    'drm_nouveau',
    'modplug',
    'nx',
    'png16',
    'SDL2main',
]

foreach dep : dependencies
    deps += dependency(
        dep,
        required: true,
        allow_fallback: false,
        native: false,
    )
endforeach

library_dirs = meson.get_external_property('library_dirs', [''])
if library_dirs.length() == 0
    error('property \'library_dirs\' has to be set!')
endif

c = meson.get_compiler('c')
foreach dep : dependencies_native
    deps += c.find_library(
        dep,
        required: true,
        dirs: library_dirs,
    )
endforeach

## compilation

elf_file = build_target(
    exe_name + '.elf',
    src_files,
    dependencies: deps,
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    native: false,
    target_type: 'executable',
)

use_nacp = ['true', 'True', '1', true].contains(
    meson.get_external_property('USE_NACP', ''),
)

elf2nro = find_program('elf2nro')
# executable input elf file, output nro file
NRO_FLAGS = [elf2nro, elf_file.full_path(), exe_name + '.nro']
NRO_DEPS = [elf_file]
NACP_DEPS = NRO_DEPS

fs = import('fs')

if use_nacp
    nacptool = find_program('nacptool')
    NACP_FLAGS = [
        nacptool,
        '--create',
        oopetris_name,
        oopetris_author,
        meson.project_version(),
        exe_name + '.nacp',
    ]

    APP_TITLEID = meson.get_external_property('APP_TITLEID', '')

    # optional
    if APP_TITLEID != ''
        NACP_FLAGS += '--titleid=' + APP_TITLEID
    endif

    # nacptool --create <name> <author> <version> <exe_name>.nacp
    # optional: --titleid=<titleID>
    nacp_file = custom_target(
        exe_name + '.nacp',
        command: NACP_FLAGS,
        output: [exe_name + '.nacp'],
        depends: NACP_DEPS,
    )

    NRO_FLAGS += '--nacp=' + exe_name + '.nacp'
    NRO_DEPS += nacp_file

endif

LIBNX = meson.get_external_property('libnx', '')
if LIBNX == ''
    error('property \'libnx\' has to be set!')
endif

default_icon = LIBNX / 'default_icon.jpg'

APP_ICON = meson.get_external_property('APP_ICON', default_icon)

if not fs.is_absolute(APP_ICON)
    APP_ICON = meson.project_source_root() / APP_ICON
endif

if not fs.exists(APP_ICON)
    error('APP_ICON should exist, but doesn\'t: \'' + APP_ICON + '\'')
endif

NRO_FLAGS += '--icon=' + APP_ICON

APP_ROMFS = meson.get_external_property('APP_ROMFS', '')

if APP_ROMFS != ''
    if not fs.is_absolute(APP_ROMFS)
        APP_ROMFS = meson.project_source_root() / APP_ROMFS
    endif

    if not fs.exists(APP_ROMFS)
        error('APP_ROMFS should exist, but doesn\'t: \'' + APP_ROMFS + '\'')
    endif

    NRO_FLAGS += '--romfsdir=' + APP_ROMFS

endif

# elf2nro <exe_name>.elf <exe_name>.nro <NRO_FLAGS>
custom_target(
    exe_name + '.nro',
    command: NRO_FLAGS,
    depends: NRO_DEPS,
    output: [exe_name + '.nro'],
    build_by_default: true,
)
