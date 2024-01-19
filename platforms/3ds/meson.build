
# libraries

_3ds_dependencies = [
    'freetype2',
    'libpng16',
    'opusfile',
    'opus',
    'ogg',
    'libmodplug',
    'flac',
    'zlib',
    'vorbisidec',
    'ogg',
    'libmikmod',
]

_3ds_dependencies_native = ['ctru', 'bz2', 'libmad']


library_dirs = meson.get_external_property('library_dirs', [''])
if library_dirs.length() == 0
    error('property \'library_dirs\' has to be set!')
endif

foreach dep : _3ds_dependencies
    deps += dependency(
        dep,
        required: true,
        allow_fallback: false,
        native: false,
    )
endforeach

cpp = meson.get_compiler('cpp')
foreach dep : _3ds_dependencies_native
    deps += cpp.find_library(
        dep,
        required: true,
        dirs: library_dirs,
    )
endforeach




## compilation

elf_file = build_target(
    exe_name + '.elf',
    src_files,
    include_directories: inc_dirs,
    dependencies: deps,
    c_args: compile_args + sdl2_mixer_defines,
    cpp_args: compile_args + sdl2_mixer_defines,
    override_options: [
        'warning_level=3',
        'werror=true',
    ],
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
    SMDH_FLAGS = [smdhtool, '--create']

    APP_NAME = meson.get_external_property('APP_NAME', exe_name)
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

    default_icon = LIBCTRU / 'default_icon.png'

    app_icon = meson.get_external_property('APP_ICON', default_icon)

    if not fs.is_absolute(app_icon)
        app_icon = meson.project_source_root() / app_icon
    endif


    if not fs.exists(app_icon)
        error('APP_ICON should exist, but doesn\'t: \'' + app_icon + '\'')
    endif

    SMDH_FLAGS += app_icon


    SMDH_FLAGS += (
        exe_name + '.smdh' # outfile
    )



    small_app_icon = meson.get_external_property('APP_SMALL_ICON', '')

    if small_app_icon != ''
        if not fs.is_absolute(small_app_icon)
            small_app_icon = meson.project_source_root() / small_app_icon
        endif


        if not fs.exists(small_app_icon)
            error('APP_SMALL_ICON should exist, but doesn\'t: \'' + small_app_icon + '\'')
        endif

        SMDH_FLAGS += small_app_icon
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



APP_ROMFS = meson.get_external_property('romfs_dir', '')

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
