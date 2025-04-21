project(
    'discord-social-sdk',
    'cpp',
    'c',
    version: '1.1.8318',
    meson_version: '>=1.4.0',
    default_options: {
        'cpp_std': ['c++17'],
    },
)

inc_dirs = include_directories('include')

header_files = files('include' / 'cdiscord.h', 'include' / 'discordpp.h')

install_headers(
    header_files,
    subdir: 'discord',
)

lib_base_dir = ''

if host_machine.system() == 'darwin'
    if host_machine.cpu_family() in ['aarch64', 'x86_64']
        lib_base_dir = 'lib'
    else
        error('unsupported architecture for macos: ' + host_machine.cpu_family())
    endif

elif host_machine.system() == 'linux'
    if host_machine.cpu_family() == 'x86_64'
        lib_base_dir = 'lib'

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
        endif

        if linux_distro == 'alpine'
            error('unsupported libc for linux: musl')
        endif

    else
        error('unsupported architecture for linux: ' + host_machine.cpu_family())
    endif
elif host_machine.system() == 'windows'
    if host_machine.cpu_family() == 'x86_64'
        lib_base_dir = get_option('default_library') == 'static' ? 'bin' : 'lib'
    else
        error('unsupported architecture for windows: ' + host_machine.cpu_family())
    endif
else
    error('unsupported system: ' + host_machine.system())
endif

lib_dir = (lib_base_dir / (get_option('buildtype') == 'release' ? 'release' : 'debug')
)

c = meson.get_compiler('c')

discord_partner_sdk = c.find_library(
    'discord_partner_sdk',
    dirs: [meson.project_source_root() / lib_dir],
    required: true,
)

## NOTE: this is a static library, as it doesn't export the symbols correctly, so on windows the dll would fail to link
discord_social_sdk_lib = static_library(
    'discord_social_sdk',
    files('impl.cpp'),
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
