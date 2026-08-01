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

_uefi_mappings_conf = configuration_data(
    {
        'OOPETRIS_VERSION': meson.project_version(),
    }
)

_uefi_mappings_file = configure_file(
    input: 'mappings.json.in',
    output: 'mappings.json',
    configuration: _uefi_mappings_conf,
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
        _uefi_mappings_file,
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
