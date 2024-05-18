## get the options object and "unpack" it

wii_exe_name = wii_options[0]
wii_src_files = wii_options[1]
wii_deps = wii_options[2]


# libraries

wii_dependencies = [
    'freetype2',
    'harfbuzz',
    'ogg',
    'vorbis',
    'vorbisfile',
    'vorbisidec',
    'zlib',
]

wii_dependencies_native = [
    'bte',
    'bz2',
    'fat',
    'jpeg',
    'm',
    'modplug',
    'ogc',
    'png16',
    'SDL2main',
    'wiikeyboard',
    'wiiuse',
    'wut',
]

foreach dep : wii_dependencies
    wii_deps += dependency(
        dep,
        required: true,
        allow_fallback: false,
        native: false,
    )
endforeach

wii_library_dirs = meson.get_external_property('library_dirs', [''])
if wii_library_dirs.length() == 0
    error('property \'library_dirs\' has to be set!')
endif

c = meson.get_compiler('c')
foreach dep : wii_dependencies_native
    wii_deps += c.find_library(
        dep,
        required: true,
        dirs: wii_library_dirs,
    )
endforeach

## compilation

wii_elf_file = build_target(
    wii_exe_name + '.elf',
    wii_src_files,
    dependencies: wii_deps,
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
    native: false,
    target_type: 'executable',
)

DOL_DEPS = [wii_elf_file]


elf2dol = find_program('elf2dol')

# elf2dol <exe_name>.elf <exe_name>.dol 
custom_target(
    wii_exe_name + '.dol',
    command: [elf2dol, wii_elf_file.full_path(), wii_exe_name + '.dol'],
    depends: DOL_DEPS,
    output: [wii_exe_name + '.dol'],
    build_by_default: true,
)

use_meta_xml = ['true', 'True', '1', true].contains(
    meson.get_external_property('USE_META_XML', ''),
)

if use_meta_xml

    wii_meta_xml_conf = configuration_data(
        {
            'OOPETRIS_VERSION': meson.project_version(),
            'OOPETRIS_NAME': oopetris_name,
            'OOPETRIS_AUTHOR': oopetris_author,
            'OOPETRIS_SHORT_DESCRIPTION': oopetris_desc,
            'OOPETRIS_LONG_DESCRIPTION': oopetris_desc,
        },
    )

    wii_meta_xml = configure_file(
        input: 'meta.xml.in',
        output: 'meta.xml',
        configuration: wii_meta_xml_conf,
    )

endif
