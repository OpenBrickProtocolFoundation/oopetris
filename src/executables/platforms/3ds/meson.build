## get the options object and "unpack" it

exe_name = nintendo_options[0]
src_files = nintendo_options[1]
deps = nintendo_options[2]

# libraries

dependencies = [
    'flac',
    'freetype2',
    'ogg',
    'ogg',
    'opus',
    'opusfile',
    'vorbisidec',
    'zlib',
]

dependencies_native = [
    'bz2',
    'ctru',
    'mad',
    'mikmod',
    'modplug',
    'png16',
    'SDL2main',
]

library_dirs = meson.get_external_property('library_dirs', [''])
if library_dirs.length() == 0
    error('property \'library_dirs\' has to be set!')
endif

foreach dep : dependencies
    deps += dependency(
        dep,
        required: true,
        allow_fallback: false,
        native: false,
    )
endforeach

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

use_smdh = ['true', 'True', '1', true].contains(
    meson.get_external_property('USE_SMDH', ''),
)

_3dsxtool = find_program('3dsxtool')

_3DSX_FLAGS = [_3dsxtool, elf_file.full_path(), exe_name + '.3dsx']
_3DSX_DEPS = [elf_file]
SMDH_DEPS = _3DSX_DEPS

fs = import('fs')

if use_smdh

    smdhtool = find_program('smdhtool')
    SMDH_FLAGS = [
        smdhtool,
        '--create', oopetris_name,
        oopetris_desc,
        oopetris_author,
    ]

    LIBCTRU = meson.get_external_property('libctru', '')
    if LIBCTRU == ''
        error('property \'libctru\' has to be set!')
    endif

    default_icon = LIBCTRU / 'default_icon.png'

    APP_ICON = meson.get_external_property('APP_ICON', default_icon)

    if not fs.is_absolute(APP_ICON)
        APP_ICON = meson.project_source_root() / APP_ICON
    endif

    if not fs.exists(APP_ICON)
        error('APP_ICON should exist, but doesn\'t: \'' + APP_ICON + '\'')
    endif

    SMDH_FLAGS += APP_ICON

    SMDH_FLAGS += (exe_name + '.smdh')

    APP_SMALL_ICON = meson.get_external_property('APP_SMALL_ICON', '')

    if APP_SMALL_ICON != ''
        if not fs.is_absolute(APP_SMALL_ICON)
            APP_SMALL_ICON = meson.project_source_root() / APP_SMALL_ICON
        endif

        if not fs.exists(APP_SMALL_ICON)
            error(
                'APP_SMALL_ICON should exist, but doesn\'t: \''
                + APP_SMALL_ICON
                + '\'',
            )
        endif

        SMDH_FLAGS += APP_SMALL_ICON
    endif

    # smdhtool --create <name> <long description> <author> <icon.png> <exe_name>.smdh [<smallicon.png>]
    smdh_file = custom_target(
        exe_name + '.smdh',
        command: SMDH_FLAGS,
        output: [exe_name + '.smdh'],
        depends: SMDH_DEPS,
    )
    _3DSX_FLAGS += '--smdh=' + exe_name + '.smdh'
    _3DSX_DEPS += smdh_file

endif

APP_ROMFS = meson.get_external_property('APP_ROMFS', '')

if APP_ROMFS != ''

    if not fs.is_absolute(APP_ROMFS)
        APP_ROMFS = meson.project_source_root() / APP_ROMFS
    endif

    if not fs.exists(APP_ROMFS)
        error('APP_ROMFS should exist, but doesn\'t: \'' + APP_ROMFS + '\'')
    endif

    _3DSX_FLAGS += '--romfs=' + APP_ROMFS

endif

# 3dsxtool <exe_name>.elf <exe_name>.3dsx <_3DSX_FLAGS>
custom_target(
    exe_name + '.3dsx',
    command: _3DSX_FLAGS,
    depends: _3DSX_DEPS,
    output: [exe_name + '.3dsx'],
    build_by_default: true,
)
