## get the options object and "unpack" it

_uefi_exe_name = _uefi_options[0]
_uefi_src_files = _uefi_options[1]
_uefi_deps = _uefi_options[2]

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
    command: [_uefi_compile_final_exe, _uefi_extract_defs, '@OUTPUT@'],
    depends: [_uefi_extract_defs],
    output: [_uefi_exe_name + '.efi'],
    build_by_default: true,
)
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

        fat_folder = custom_target(
            'fat_folder',
            command: [_uefi_create_fat_folder_exe, fat_folder_loc, '@INPUT@', '@OUTPUT@'],
            depends: [oopetris_efi],
            input: [oopetris_efi],
            output: ['FAT_FOLDER_CREATED.meta'],
            build_by_default: false,
        )


        run_target(
            'emulate_efi',
            command: [
                _qemu_system_exe,
                '-cpu',
                'qemu64',
                '-drive',
                'if=pflash,format=raw,unit=0,file=' + OVMF_CODE_fd_path + ',readonly=on',
                '-drive',
                'if=pflash,format=raw,unit=1,file=' + OVMF_VARS_fd_path,
                '-drive',
                'format=raw,file=fat:rw:' + fat_folder_loc,
                '-net',
                'none',
            ],
            depends: [oopetris_efi, fat_folder],
        )
    else
        warning('OVMF files couldn\'t be found: target for running efi not created')
    endif
else
    warning('qemu couldn\'t be found: target for running efi not created')
endif
