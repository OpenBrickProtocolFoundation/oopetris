graphics_src_files += files(
    'clock_source.cpp',
    'clock_source.hpp',
    'constants.hpp',
    'export_symbols.hpp',
    'git_helper.hpp',
    'graphic_utils.cpp',
    'graphic_utils.hpp',
    'message_box.cpp',
    'message_box.hpp',
    'music_utils.hpp',
    'platform.cpp',
    'platform.hpp',
    'spdlog_wrapper.hpp',
)

if meson.is_cross_build() and host_machine.system() == 'uefi'
    graphics_src_files += files(
        'uefi_futures.cpp',
        'uefi_futures.hpp',
        'uefi_utils.cpp',
        'uefi_utils.hpp',
        'uefi_utils_spdlog_sink.cpp',
    )
elif meson.is_cross_build() and host_machine.system() == 'emscripten'
    graphics_src_files += files(
        'web_utils.cpp',
        'web_utils.hpp',
    )
elif meson.is_cross_build() and (
    host_machine.system() == '3ds'
    or host_machine.system() == 'switch'
)
    graphics_src_files += files(
        'console_helpers.cpp',
        'console_helpers.hpp',
    )

endif



if have_file_dialogs
    graphics_src_files += files('nfd.cpp', 'nfd_include.hpp')
endif

git = find_program('git')

vcs_dep = vcs_tag(
    command: [git, 'describe', '--tags', '--always', '--abbrev=12'],
    input: 'git_version.hpp.in',
    output: 'git_version.hpp',
    replace_string: '@GIT_VERSION@',
)

graphics_src_files += vcs_dep
