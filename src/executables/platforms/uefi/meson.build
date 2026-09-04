## get the options object and "unpack" it

_uefi_exe_name = _uefi_options[0]
_uefi_src_files = _uefi_options[1]
_uefi_deps = _uefi_options[2]


APP_ROMFS = meson.get_external_property('APP_ROMFS', '')

if APP_ROMFS != ''
    # create uefi_rom.o and add it as dependency

    fs = import('fs')

    if not fs.is_absolute(APP_ROMFS)
        APP_ROMFS = meson.project_source_root() / APP_ROMFS
    endif

    if not fs.exists(APP_ROMFS)
        error('APP_ROMFS should exist, but doesn\'t: \'' + APP_ROMFS + '\'')
    endif

    if not APP_ROMFS.endswith('/')
        APP_ROMFS = APP_ROMFS + '/'
    endif

    __uefi_c_embed_exe = find_program(
        'c-embed',
        required: true,
        native: true,
    )

    _uefi_c_embed_wrapper_exe = find_program(
        'c-embed-wrapper.sh',
        dirs: [meson.project_source_root() / 'platforms' / 'uefi'],
        required: true,
    )

    _uefi_rom_o = custom_target(
        'uefi_rom.o',
        command: [
            _uefi_c_embed_wrapper_exe,
            __uefi_c_embed_exe,
            APP_ROMFS,
            '@OUTPUT@',
        ],
        output: ['uefi_rom.o'],
        build_by_default: false,
    )

    _uefi_rom_lib = library(
        'uefi_rom',
        sources: _uefi_rom_o,
        link_language: 'c',
    )

    _uefi_rom_dep = declare_dependency(
        link_with: _uefi_rom_lib
    )

    _uefi_deps += _uefi_rom_dep


endif


## compilation

_uefi_final_description = executable(
    _uefi_exe_name,
    _uefi_src_files,
    dependencies: _uefi_deps,
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
)

_uefi_extract_defs_exe = find_program(
    'extract-definitions.sh',
    dirs: [meson.project_source_root() / 'platforms' / 'uefi'],
    required: true,
)

_uefi_extract_defs = custom_target(
    _uefi_exe_name + '.json',
    command: [
        _uefi_extract_defs_exe,
        _uefi_final_description,
        '@OUTPUT@',
    ],
    depends: [_uefi_final_description],
    output: ['OOPetrisPkg.inf'],
    build_by_default: true,
)

_uefi_compile_final_exe = find_program(
    'compile-final.sh',
    dirs: [meson.project_source_root() / 'platforms' / 'uefi'],
    required: true,
)

oopetris_efi = custom_target(
    _uefi_exe_name + '.efi',
    command: [_uefi_compile_final_exe, _uefi_extract_defs, '@OUTPUT@', '@DEPFILE@'],
    depends: [_uefi_extract_defs],
    depfile: _uefi_exe_name + '.efi.deps',
    output: [_uefi_exe_name + '.efi'],
    build_by_default: true,
)

RUNTIME_TARGET = meson.get_external_property('RUNTIME_TARGET', '')

if RUNTIME_TARGET == 'hardware'
    # nothing to do
elif RUNTIME_TARGET == 'emulator'
    # add some qemu related targets
    _qemu_system_exe = find_program(
        'qemu-system-x86_64',
        required: false,
    )


    if _qemu_system_exe.found()
        cat = find_program('cat')

        os_release_info = run_command(
            cat,
            '/etc/os-release',
            check: true,
        ).stdout().strip().split('\n')

        linux_distro = ''

        foreach line : os_release_info
            line_detail = line.split('=')

            if line_detail[0] == 'ID'
                linux_distro = line_detail[1]
            endif

        endforeach

        if linux_distro == ''
            warning('Couldn\'t detect the linux distro')
            OVMF_CODE_fd_path = '/usr/share/OVMF/OVMF_CODE.fd'
            OVMF_VARS_fd_path = '/usr/share/OVMF/OVMF_VARS.fd'
        elif linux_distro == 'ubuntu'
            OVMF_CODE_fd_path = '/usr/share/OVMF/OVMF_CODE_4M.fd'
            OVMF_VARS_fd_path = '/usr/share/OVMF/OVMF_VARS_4M.fd'
        else
            OVMF_CODE_fd_path = '/usr/share/OVMF/OVMF_CODE.fd'
            OVMF_VARS_fd_path = '/usr/share/OVMF/OVMF_VARS.fd'
        endif

        fs = import('fs')


        if fs.exists(OVMF_CODE_fd_path) and fs.exists(OVMF_VARS_fd_path)

            _uefi_create_fat_folder_exe = find_program(
                'create-fat-folder.sh',
                dirs: [meson.project_source_root() / 'platforms' / 'uefi'],
                required: true,
            )

            fat_folder_loc = fs.parent(oopetris_efi.full_path()) / 'FAT_FOLDER'

            serial_debug_file = fs.parent(oopetris_efi.full_path()) / 'serial_debug.log'

            fat_folder = custom_target(
                'fat_folder',
                command: [_uefi_create_fat_folder_exe, fat_folder_loc, '@INPUT@', '@OUTPUT@'],
                depends: [oopetris_efi],
                input: [oopetris_efi],
                output: ['FAT_FOLDER_CREATED.meta'],
                build_by_default: false,
            )

            user_vars_fd = fs.copyfile(
                OVMF_VARS_fd_path,
                'OVMF_VARS.fd',
            )

            emulate_command = [
                _qemu_system_exe,
                # this machine supports the tcd timer frequency ACPI getter call
                '-machine',
                'q35',
                # see https://www.qemu.org/docs/master/system/qemu-cpu-models.html#id1
                # this cpu has x86-64 v1 (baseline) support
                '-cpu',
                'qemu64-v1',
                '-smp',
                'sockets=1,cores=4,threads=1',
                '-drive',
                'if=pflash,format=raw,unit=0,file=' + OVMF_CODE_fd_path + ',readonly=on',
                '-drive',
                'if=pflash,format=raw,unit=1,file=' + user_vars_fd.full_path(),
                '-drive',
                'format=raw,file=fat:rw:' + fat_folder_loc,
                '-net',
                'none',
            ]

            if get_option('buildtype') != 'release'
                emulate_command += [
                    '-debugcon',
                    'stdio',
                    '-global',
                    'isa-debugcon.iobase=0x402',
                    '-serial',
                    'file:' + serial_debug_file,
                ]


                use_gdb = ['true', 'True', '1', true].contains(
                    meson.get_external_property('USE_GDB', '')
                )

                if use_gdb
                    emulate_command += [
                        '-s',
                        '-S',
                    ]
                endif

            endif



            run_target(
                'emulate_efi',
                command: emulate_command,
                depends: [oopetris_efi, fat_folder],
            )
        else
            warning('OVMF files couldn\'t be found: target for running efi not created')
        endif
    else
        warning('qemu couldn\'t be found: target for running efi not created')
    endif
else
    error('RUNTIME_TARGET has invalid value: \'' + RUNTIME_TARGET + '\'')
endif
