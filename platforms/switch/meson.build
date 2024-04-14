
## get the options object and "unpack" it

switch_exe_name = switch_options[0]
switch_src_files = switch_options[1]
switch_deps = switch_options[2]


# libraries

switch_dependencies = [
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

switch_dependencies_native = [
    'bz2',
    'drm_nouveau',
    'modplug',
    'nx',
    'png16',
    'SDL2main',
]


foreach dep : switch_dependencies
    switch_deps += dependency(
        dep,
        required: true,
        allow_fallback: false,
        native: false,
    )
endforeach

switch_library_dirs = meson.get_external_property('library_dirs', [''])
if switch_library_dirs.length() == 0
    error('property \'library_dirs\' has to be set!')
endif

c = meson.get_compiler('c')
foreach dep : switch_dependencies_native
    switch_deps += c.find_library(
        dep,
        required: true,
        dirs: switch_library_dirs,
    )
endforeach



## compilation

switch_elf_file = build_target(
    switch_exe_name + '.elf',
    switch_src_files,
    dependencies: switch_deps,
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
NRO_FLAGS = [elf2nro, switch_elf_file.full_path(), switch_exe_name + '.nro']
NRO_DEPS = [switch_elf_file]
NACP_DEPS = NRO_DEPS

fs = import('fs')

if use_nacp
    nacptool = find_program('nacptool')
    NACP_FLAGS = [nacptool, '--create']

    APP_NAME = meson.get_external_property('APP_NAME', switch_exe_name)
    NACP_FLAGS += APP_NAME


    APP_AUTHOR = meson.get_external_property('APP_AUTHOR', '')
    if APP_AUTHOR == ''
        error('If USE_NACP is set, you have to provide an APP_AUTHOR')
    endif
    NACP_FLAGS += APP_AUTHOR

    APP_VERSION = meson.get_external_property('APP_VERSION', '')
    if APP_VERSION == ''
        error('If USE_NACP is set, you have to provide an APP_VERSION')
    endif
    NACP_FLAGS += APP_VERSION

    NACP_FLAGS += (
        switch_exe_name + '.nacp' # outfile
    )


    APP_TITLEID = meson.get_external_property('APP_TITLEID', '')

    # optional
    if APP_TITLEID != ''
        NACP_FLAGS += '--titleid=' + APP_TITLEID
    endif


    # nacptool --create <name> <author> <version> <switch_exe_name>.nacp 
    # optional: --titleid=<titleID>
    nacp_file = custom_target(
        switch_exe_name + '.nacp',
        command: NACP_FLAGS,
        output: [switch_exe_name + '.nacp'],
        depends: NACP_DEPS,
    )
    NRO_FLAGS += '--nacp=' + switch_exe_name + '.nacp'
    NRO_DEPS += nacp_file


endif

LIBNX = meson.get_external_property('libnx', '')
if LIBNX == ''
    error('property \'libnx\' has to be set!')
endif

switch_default_icon = LIBNX / 'default_icon.jpg'

APP_ICON = meson.get_external_property('APP_ICON', switch_default_icon)

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

# elf2nro <switch_exe_name>.elf <switch_exe_name>.nro <NRO_FLAGS>
custom_target(
    switch_exe_name + '.nro',
    command: NRO_FLAGS,
    depends: NRO_DEPS,
    output: [switch_exe_name + '.nro'],
    build_by_default: true,
)
