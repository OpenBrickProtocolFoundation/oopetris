##note: this file "parses" and uses the prefab data from the discord social sdk arr, read more on the prefab concept here:  https://developer.android.com/build/native-dependencies

# this isn't feature complete and doesn't care about actual defintion and config files, it uses a fixed static layout from the .aar file and uses it hard coded

## see: https://discord.com/developers/docs/social-sdk/md_pages_2mobile.html
# on how discord for android works

aar_file = meson.project_source_root() / 'lib' / (
    get_option('buildtype') == 'release' ? 'release' : 'debug'
) / 'discord_partner_sdk.aar'

unzip = find_program('unzip')

unzip_base_dir = meson.current_source_dir()

destination_directory = unzip_base_dir / 'prefab' / 'discord'

message(destination_directory)

fs = import('fs')

if not fs.exists(destination_directory)

    mkdir = find_program('mkdir')

    run_command(
        mkdir,
        '-pv',
        destination_directory,
        check: true,
    )

    run_command(
        unzip,
        aar_file,
        '-d',
        destination_directory,
        check: true,
    )

endif

base_prefab_dir = destination_directory / 'prefab' / 'modules' / 'discord_partner_sdk'

inc_dirs = include_directories(fs.relative_to(base_prefab_dir, unzip_base_dir) / 'include')

header_files = files(
    base_prefab_dir / 'include' / 'cdiscord.h',
    base_prefab_dir / 'include' / 'discordpp.h',
)

install_headers(
    header_files,
    subdir: 'discord',
)

arch_name = host_machine.cpu_family()

if host_machine.cpu_family() == 'arm'
    arch_name = 'armeabi-v7a'
elif host_machine.cpu_family() == 'aarch64'
    arch_name = 'arm64-v8a'
elif host_machine.cpu_family() == 'x86' or host_machine.cpu_family() == 'x86_64'
    # name is already correct
else
    error('unsupported android architecture: ' + host_machine.cpu_family())
endif

lib_dir = (base_prefab_dir / 'libs' / ('android.' + arch_name))

c = meson.get_compiler('c')

discord_partner_sdk = c.find_library(
    'discord_partner_sdk',
    dirs: [lib_dir],
    required: true,
)

discord_social_sdk_lib = static_library(
    'discord_social_sdk',
    files('../impl.cpp'),
    include_directories: inc_dirs,
    dependencies: discord_partner_sdk,
    install: true,
)

discord_social_sdk_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
    link_with: discord_social_sdk_lib,
)

meson.override_dependency('discord-social-sdk', discord_social_sdk_dep)
