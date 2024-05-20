## get the options object and "unpack" it

exe_name = nintendo_options[0]
src_files = nintendo_options[1]
deps = nintendo_options[2]


# libraries

dependencies = [
    'freetype2',
    'harfbuzz',
    'ogg',
    'vorbis',
    'vorbisfile',
    'vorbisidec',
    'zlib',
]

dependencies_native = [
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

DOL_DEPS = [elf_file]


elf2dol = find_program('elf2dol')

# elf2dol <exe_name>.elf <exe_name>.dol 
custom_target(
    exe_name + '.dol',
    command: [elf2dol, elf_file.full_path(), exe_name + '.dol'],
    depends: DOL_DEPS,
    output: [exe_name + '.dol'],
    build_by_default: true,
)

use_meta_xml = ['true', 'True', '1', true].contains(
    meson.get_external_property('USE_META_XML', ''),
)

if use_meta_xml

    meta_xml_conf = configuration_data(
        {
            'OOPETRIS_VERSION': meson.project_version(),
            'OOPETRIS_NAME': oopetris_name,
            'OOPETRIS_AUTHOR': oopetris_author,
            'OOPETRIS_SHORT_DESCRIPTION': oopetris_desc,
            'OOPETRIS_LONG_DESCRIPTION': oopetris_desc,
        },
    )

    meta_xml = configure_file(
        input: 'meta.xml.in',
        output: 'meta.xml',
        configuration: meta_xml_conf,
    )

endif
