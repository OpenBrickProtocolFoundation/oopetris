project(
    'libromfs',
    'c',
    version: '0.7.0',
    meson_version: '>=1.2.0',
    default_options: {
        'c_std': ['c11'],
    },
)

deps = []


if meson.is_cross_build()
    if host_machine.system() == 'wiiu'

        object_target = 'elf32-powerpc'
        binary_arch = 'powerpc:common'
        ## deps +='wut' 

    elif host_machine.system() == 'wii'

        object_target = 'elf32-powerpc'
        binary_arch = 'powerpc:common'

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

tar = find_program('tar', native: true)

objcopy = find_program('objcopy', native: false)

romfs_tar_name = 'romfs.tar'
romfs_obj_name = 'romfs.o'


romfs_tar = custom_target(
    romfs_tar_name,
    command: [
        tar,
        '--format', 'ustar',
        '-cvf', '@OUTPUT@',
        '-C', romfs_dir,
        '.',
    ],
    output: [romfs_tar_name],
)

romfs_object = custom_target(
    romfs_obj_name,
    command: [
        objcopy,
        '--input-target', 'binary',
        '--output-target', object_target,
        '--binary-architecture', binary_arch,
        '@INPUT@',
        '@OUTPUT@',
    ],
    input: romfs_tar,
    output: [romfs_obj_name],
)

inc_dirs = include_directories('include')

libromfs_lib = library(
    'libromfs',
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
