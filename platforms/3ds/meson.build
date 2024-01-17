
# libraries

_3ds_dependencies = [
    'ctru',
    'freetype',
    'z',
    'bz2',
    'png',
    'opusfile',
    'opus',
    'ogg',
    'modplug',
    'FLAC',
]


library_dirs = meson.get_external_property('library_dirs')

foreach dep : _3ds_dependencies
    deps += cpp.find_library(
        dep,
        required: true,
        dirs: library_dirs,
    )
endforeach



## compilation

elf_file = build_target(
    'oopetris_3ds.elf',
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


_3dsxtool = find_program('3dsxtool')

_3DSX_FLAGS = [_3dsxtool, elf_file.full_path(), 'oopetris.3dsx']
_3DSX_DEPS = [elf_file]
SMDH_DEPS = _3DSX_DEPS

fs = import('fs')


    smdhtool = find_program('smdhtool')
    SMDH_FLAGS = [smdhtool, '--create']

    APP_NAME = meson.get_external_property('APP_NAME', 'oopetris')
    SMDH_FLAGS += APP_NAME

    APP_DESC = meson.get_external_property('APP_DESC', '')
    if APP_DESC == ''
        error('You have to provide an APP_DESC')
    endif
    SMDH_FLAGS += APP_DESC

    default_icon = meson.get_external_property('libctru') / 'default_icon.png'

    app_icon = meson.get_external_property('APP_ICON', default_icon)

    if not fs.is_absolute(app_icon)
        app_icon = meson.project_source_root() / app_icon
    endif


    if not fs.exists(app_icon)
        error('APP_ICON should exist, but doesn\'t: \'' + app_icon + '\'')
    endif

    SMDH_FLAGS +=  app_icon

    APP_AUTHOR = meson.get_external_property('APP_AUTHOR', '')
    if APP_AUTHOR == ''
        error('You have to provide an APP_AUTHOR')
    endif
    SMDH_FLAGS += APP_AUTHOR

    APP_VERSION = meson.get_external_property('APP_VERSION', '')
    if APP_VERSION == ''
        error('You have to provide an APP_VERSION')
    endif
    SMDH_FLAGS += APP_VERSION


    SMDH_FLAGS += 'oopetris.smdh' # outfile



    small_app_icon = meson.get_external_property('APP_SMALL_ICON', default_icon)

    if small_app_icon != ''
        if not fs.is_absolute(small_app_icon)
            small_app_icon = meson.project_source_root() / small_app_icon
        endif


        if not fs.exists(small_app_icon)
            error('APP_SMALL_ICON should exist, but doesn\'t: \'' + small_app_icon + '\'')
        endif
        
        SMDH_FLAGS +=  small_app_icon
    endif



    # smdhtool --create <name> <long description> <author> <icon.png> oopetris.smdh [<smallicon.png>]
    smdh_file = custom_target(
        'oopetris.smdh',
        command: SMDH_FLAGS,
        output: ['oopetirs.smdh'],
        depends: SMDH_DEPS,
    )
    _3DSX_FLAGS += '--smdh=oopetris.smdh'
    _3DSX_DEPS += smdh_file





romfs_dir = meson.get_external_property('romfs_dir')

if not fs.is_absolute(romfs_dir)
    romfs_dir = meson.project_source_root() / romfs_dir
endif

if not fs.exists(romfs_dir)
    error('romfs_dir should exist, but doesn\'t: \'' + romfs_dir + '\'')
endif




_3DSX_FLAGS += '--romfs=' + romfs_dir

# 3dsxtool oopetris.elf oopetris.3dsx <_3DSX_FLAGS>
custom_target(
    'oopetris.3dsx',
    command: _3DSX_FLAGS,
    depends: _3DSX_DEPS,
    output: ['oopetris.3dsx'],
    build_by_default: true,
)
