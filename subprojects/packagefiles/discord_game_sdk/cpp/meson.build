inc_dirs = include_directories('.')

header_files = files(
    'achievement_manager.h',
    'activity_manager.h',
    'application_manager.h',
    'core.h',
    'discord.h',
    'event.h',
    'ffi.h',
    'image_manager.h',
    'lobby_manager.h',
    'network_manager.h',
    'overlay_manager.h',
    'relationship_manager.h',
    'storage_manager.h',
    'store_manager.h',
    'types.h',
    'user_manager.h',
    'voice_manager.h',
)

install_headers(header_files, subdir: 'discord')

src_files = files(
    'achievement_manager.cpp',
    'activity_manager.cpp',
    'application_manager.cpp',
    'core.cpp',
    'image_manager.cpp',
    'lobby_manager.cpp',
    'network_manager.cpp',
    'overlay_manager.cpp',
    'relationship_manager.cpp',
    'storage_manager.cpp',
    'store_manager.cpp',
    'types.cpp',
    'user_manager.cpp',
    'voice_manager.cpp',
)

lib_dir = ''
dynamic_lib = ''

if host_machine.system() == 'darwin'
    dynamic_lib = 'discord_game_sdk.dylib'
    if host_machine.cpu_family() == 'aarch64'
        lib_dir = 'lib' / 'aarch64'
    elif host_machine.cpu_family() == 'x86_64'
        lib_dir = 'lib' / 'x86_64'
    else
        error('unsuported architectue for macos: ' + host_machine.cpu_family())
    endif

elif host_machine.system() == 'linux'
    dynamic_lib = 'discord_game_sdk.so'
    if host_machine.cpu_family() == 'x86_64'
        lib_dir = 'lib' / 'x86_64'
    else
        error('unsuported architectue for linux: ' + host_machine.cpu_family())
    endif
elif host_machine.system() == 'windows'
    dynamic_lib = 'discord_game_sdk.dll'
    if host_machine.cpu_family() == 'x86'
        lib_dir = 'lib' / 'x86'
    elif host_machine.cpu_family() == 'x86_64'
        lib_dir = 'lib' / 'x86_64'
    else
        error('unsuported architectue for windows: ' + host_machine.cpu_family())
    endif
else
    error('unsuported system: ' + host_machine.system())
endif

c = meson.get_compiler('c')

lib_deps = []

## prefrerring the static variant, if not found we need to copy the danmic lib to the libdir on installation!

lib_name = 'discord_game_sdk'

if c.get_id() == 'msvc'
    lib_name = 'discord_game_sdk.dll'
endif

discord_lib_c = c.find_library(
    lib_name,
    dirs: [meson.project_source_root() / lib_dir],
    static: true,
    required: false,
)

if discord_lib_c.found()
    lib_deps += discord_lib_c

    if c.get_id() == 'msvc'

        install_data(
            meson.project_source_root() / lib_dir / lib_name,
            install_dir: get_option('bindir'),
        )
    endif

else
    lib_deps += c.find_library(
        lib_name,
        dirs: [meson.project_source_root() / lib_dir],
    )

    dynamic_lib_rename = dynamic_lib

    if host_machine.system() == 'linux'
        dynamic_lib_rename = 'libdiscord_game_sdk_c.so'

        objdump = find_program('objdump')

        objdump_result = run_command(
            objdump,
            '-p', meson.project_source_root() / lib_dir / dynamic_lib,
            check: true,
        ).stdout().strip()

        if objdump_result.contains('SONAME')
            message('discord game sdk has a SONAME')

        else
            message('discord game sdk has no SONAME')

            patchelf = find_program('patchelf')

            #TODO: file issue to discord, to set SONAME
            run_command(
                patchelf,
                '--set-soname', dynamic_lib_rename,
                meson.project_source_root() / lib_dir / dynamic_lib,
                check: true,
            )

            run_command(
                find_program('cp'),
                meson.project_source_root() / lib_dir / dynamic_lib,
                meson.project_source_root() / lib_dir / dynamic_lib_rename,
                check: true,
            )

        endif
    endif

    install_data(
        meson.project_source_root() / lib_dir / dynamic_lib,
        install_dir: get_option('libdir'),
        rename: [dynamic_lib_rename],
    )

endif

discord_lib = library(
    'discord-game-sdk',
    src_files,
    header_files,
    include_directories: inc_dirs,
    dependencies: lib_deps,
    install: true,
)

discord_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
    link_with: discord_lib,
)
