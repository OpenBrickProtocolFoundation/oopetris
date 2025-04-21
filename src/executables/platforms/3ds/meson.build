## get the options object and "unpack" it

_3ds_exe_name = _3ds_options[0]
_3ds_src_files = _3ds_options[1]
_3ds_deps = _3ds_options[2]

# libraries

_3ds_dependencies = ['flac', 'freetype2', 'ogg', 'ogg', 'opus', 'opusfile', 'vorbisidec', 'zlib']

_3ds_dependencies_native = ['bz2', 'ctru', 'mad', 'mikmod', 'modplug', 'png16', 'SDL2main']

_3ds_library_dirs = meson.get_external_property('library_dirs', [''])
if _3ds_library_dirs.length() == 0
    error('property \'library_dirs\' has to be set!')
endif

foreach dep : _3ds_dependencies
    _3ds_deps += dependency(
        dep,
        required: true,
        allow_fallback: false,
        native: false,
    )
endforeach

c = meson.get_compiler('c')
foreach dep : _3ds_dependencies_native
    _3ds_deps += c.find_library(
        dep,
        required: true,
        dirs: _3ds_library_dirs,
    )
endforeach

## compilation

_3ds_elf_file = build_target(
    _3ds_exe_name + '.elf',
    _3ds_src_files,
    dependencies: _3ds_deps,
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    native: false,
    target_type: 'executable',
)

use_smdh = ['true', 'True', '1', true].contains(meson.get_external_property('USE_SMDH', ''))

_3dsxtool = find_program('3dsxtool')

_3DSX_FLAGS = [_3dsxtool, _3ds_elf_file.full_path(), _3ds_exe_name + '.3dsx']
_3DSX_DEPS = [_3ds_elf_file]
SMDH_DEPS = _3DSX_DEPS

fs = import('fs')

if use_smdh

    smdhtool = find_program('smdhtool')
    SMDH_FLAGS = [smdhtool, '--create']

    APP_NAME = meson.get_external_property('APP_NAME', _3ds_exe_name)
    SMDH_FLAGS += APP_NAME

    APP_DESC = meson.get_external_property('APP_DESC', '')
    if APP_DESC == ''
        error('If USE_SMDH is set, you have to provide an APP_DESC')
    endif
    SMDH_FLAGS += APP_DESC

    APP_AUTHOR = meson.get_external_property('APP_AUTHOR', '')
    if APP_AUTHOR == ''
        error('If USE_SMDH is set, you have to provide an APP_AUTHOR')
    endif
    SMDH_FLAGS += APP_AUTHOR

    LIBCTRU = meson.get_external_property('libctru', '')
    if LIBCTRU == ''
        error('property \'libctru\' has to be set!')
    endif

    _3ds_default_icon = LIBCTRU / 'default_icon.png'

    APP_ICON = meson.get_external_property('APP_ICON', _3ds_default_icon)

    if not fs.is_absolute(APP_ICON)
        APP_ICON = meson.project_source_root() / APP_ICON
    endif

    if not fs.exists(APP_ICON)
        error('APP_ICON should exist, but doesn\'t: \'' + APP_ICON + '\'')
    endif

    SMDH_FLAGS += APP_ICON

    SMDH_FLAGS += (_3ds_exe_name + '.smdh')

    APP_SMALL_ICON = meson.get_external_property('APP_SMALL_ICON', '')

    if APP_SMALL_ICON != ''
        if not fs.is_absolute(APP_SMALL_ICON)
            APP_SMALL_ICON = meson.project_source_root() / APP_SMALL_ICON
        endif

        if not fs.exists(APP_SMALL_ICON)
            error('APP_SMALL_ICON should exist, but doesn\'t: \''
                + APP_SMALL_ICON + '\'')
        endif

        SMDH_FLAGS += APP_SMALL_ICON
    endif

    # smdhtool --create <name> <long description> <author> <icon.png> <_3ds_exe_name>.smdh [<smallicon.png>]
    smdh_file = custom_target(
        _3ds_exe_name + '.smdh',
        command: SMDH_FLAGS,
        output: [_3ds_exe_name + '.smdh'],
        depends: SMDH_DEPS,
    )
    _3DSX_FLAGS += '--smdh=' + _3ds_exe_name + '.smdh'
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

# 3dsxtool <_3ds_exe_name>.elf <_3ds_exe_name>.3dsx <_3DSX_FLAGS>
custom_target(
    _3ds_exe_name + '.3dsx',
    command: _3DSX_FLAGS,
    depends: _3DSX_DEPS,
    output: [_3ds_exe_name + '.3dsx'],
    build_by_default: true,
)
