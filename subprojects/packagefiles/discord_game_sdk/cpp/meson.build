
inc_dirs = include_directories('.')

src_files = files(
    'achievement_manager.cpp',
    'achievement_manager.h',
    'activity_manager.cpp',
    'activity_manager.h',
    'application_manager.cpp',
    'application_manager.h',
    'core.cpp',
    'core.h',
    'discord.h',
    'event.h',
    'ffi.h',
    'image_manager.cpp',
    'image_manager.h',
    'lobby_manager.cpp',
    'lobby_manager.h',
    'network_manager.cpp',
    'network_manager.h',
    'overlay_manager.cpp',
    'overlay_manager.h',
    'relationship_manager.cpp',
    'relationship_manager.h',
    'storage_manager.cpp',
    'storage_manager.h',
    'store_manager.cpp',
    'store_manager.h',
    'types.cpp',
    'types.h',
    'user_manager.cpp',
    'user_manager.h',
    'voice_manager.cpp',
    'voice_manager.h',
)

lib_dir = ''

if host_machine.system() == 'darwin'
    if host_machine.cpu_family() == 'aarch64'
        lib_dir = 'lib' / 'aarch64'
    elif host_machine.cpu_family() == 'x86_64'
        lib_dir = 'lib' / 'x86_64'
    else
        error('unsuported architectue for macos: ' + host_machine.cpu_family())
    endif

elif host_machine.system() == 'linux'
    if host_machine.cpu_family() == 'x86_64'
        lib_dir = 'lib' / 'x86_64'
    else
        error('unsuported architectue for linux: ' + host_machine.cpu_family())
    endif
elif host_machine.system() == 'windows'
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

discord_lib_c = c.find_library('discord_game_sdk', dirs: [meson.project_source_root() / lib_dir])

discord_lib = library(
    'discord-game-sdk',
    src_files,
    include_directories: inc_dirs,
    dependencies: discord_lib_c,
)

discord_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
    link_with: discord_lib,
)

