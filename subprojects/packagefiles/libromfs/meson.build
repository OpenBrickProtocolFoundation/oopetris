project(
    'libromfs',
    'c',
    version: '0.7.0',
    meson_version: '>=1.2.0',
    default_options: {
        'c_std': ['gnu11'],
    },
)

deps = []

if meson.is_cross_build()
    if (
        host_machine.system() == 'wiiu'
        or host_machine.system() == 'wii'
        or host_machine.system() == 'gamecube'
    )

        object_target = 'elf32-powerpc'
        binary_arch = 'powerpc:common'

        library_dirs = meson.get_external_property('library_dirs', [''])
        if library_dirs.length() == 0
            error('property \'library_dirs\' has to be set!')
        endif

        c = meson.get_compiler('c')

        if host_machine.system() == 'wiiu'
            deps += c.find_library(
                'wut',
                required: true,
                dirs: library_dirs,
            )
        elif (
            host_machine.system() == 'wii'
            or host_machine.system() == 'gamecube'
        )

            deps += c.find_library(
                'ogc',
                required: true,
                dirs: library_dirs,
            )
        endif

    else
        error('not supported platform: ' + host_machine.system())
    endif
else
    error('only cross build is supported')
endif

romfs_dir = get_option('romfs_dir')

if romfs_dir == ''
    error('\'romfs_dir\' option is required')
endif

fs = import('fs')

if not fs.exists(romfs_dir)
    error('\'romfs_dir\' should exist, but doesn\'t: \'' + romfs_dir + '\'')
endif

message('Using folder \'' + romfs_dir + '\' for ROMFS')


find = find_program('find', native: true)

romfs_files = run_command(find, romfs_dir, check: true).stdout().strip().split(
    '\n',
)

tar = find_program('tar', native: true)

objcopy = find_program('objcopy', native: false)

romfs_tar_name = 'romfs.tar'
romfs_obj_name = 'romfs.o'

romfs_tar = custom_target(
    depend_files: romfs_files,
    command: [
        tar,
        '--format', 'ustar',
        '-cvf', '@OUTPUT@',
        '-C', romfs_dir,
        '.',
    ],
    output: [romfs_tar_name],
)

binary_start_name = (
    '_binary' + (
        romfs_tar.full_path().replace(meson.global_build_root(), '').replace(
            '/',
            '_',
        ).replace('.', '_').replace('-', '_')
    )
)
binary_final_name = '_binary_romfs_tar'

romfs_object = custom_target(
    romfs_obj_name,
    command: [
        objcopy,
        '--input-target', 'binary',
        '--output-target', object_target,
        '--binary-architecture', binary_arch,
        '--redefine-sym', binary_start_name + '_start=' + binary_final_name + '_start',
        '--redefine-sym', binary_start_name + '_end=' + binary_final_name + '_end',
        '--redefine-sym', binary_start_name + '_size=' + binary_final_name + '_size',
        '@INPUT@',
        '@OUTPUT@',
    ],
    input: romfs_tar,
    output: [romfs_obj_name],
)

inc_dirs = include_directories('include')

libromfs_lib = library(
    'romfs',
    files('source/romfs.c'),
    include_directories: inc_dirs,
    dependencies: deps,
    install: true,
    objects: romfs_object,
)

libromfs_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
    link_with: libromfs_lib,
)
