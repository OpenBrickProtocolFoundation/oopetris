only_allow_native_libs = false
if meson.is_cross_build()
    if host_machine.system() == 'switch' or host_machine.system() == '3ds'
        # we do not link to code that was compiled with gcc 10.1 / gcc 7.1, the code we link with is all compiled with gcc 13.2
        core_lib += {
            'compile_args': [core_lib.get('compile_args'), '-Wno-psabi'],
        }

        only_allow_native_libs = true

        # TODO: debug, if exceptions or rtti work on the 3ds

        # if host_machine.system() == '3ds'
        #     add_global_link_arguments(
        #         '-fno-rtti',
        #         '-fno-exceptions',
        #         language: ['cpp'],
        #     )
        #     add_global_arguments(
        #         '-fno-rtti',
        #         '-fno-exceptions',
        #         language: ['cpp'],
        #     )
        # endif

    endif
endif

sdl2_dep = dependency(
    'sdl2',
    'SDL2',
    allow_fallback: false,
    required: only_allow_native_libs,
    version: '>=2.26.0',
)

if sdl2_dep.found()
    graphics_lib += {
        'deps': [graphics_lib.get('deps'), sdl2_dep],
    }
else
    sdl2_dep = dependency(
        'sdl2',
        required: true,
        default_options: {'test': false},
        version: '>=2.24.0',
    )
    sdl2main_dep = dependency(
        'sdl2main',
        required: true,
        fallback: 'sdl2',
        version: '>=2.24.0',
    )

    graphic_application_deps += sdl2main_dep

    graphics_lib += {
        'deps': [graphics_lib.get('deps'), sdl2_dep],
    }
endif

sdl2_ttf_dep = dependency(
    'sdl2_ttf',
    'SDL2_ttf',
    allow_fallback: not only_allow_native_libs,
    required: true,
)
graphics_lib += {
    'deps': [graphics_lib.get('deps'), sdl2_ttf_dep],
}

sdl2_image_dep = dependency(
    'sdl2_image',
    'SDL2_image',
    allow_fallback: not only_allow_native_libs,
    required: true,
)
graphics_lib += {
    'deps': [graphics_lib.get('deps'), sdl2_image_dep],
}

# a dirty thing atm, until mpg123 is ported to meson (maybe never...)
mpg123_dep = dependency(
    'mpg123',
    allow_fallback: true,
    required: false,
)
sdl2_mixer_flags = {'flac': 'enabled'}
sdl2_mixer_defines = ['-DAUDIO_WITH_FLAC_SUPPORT']
if mpg123_dep.found()
    sdl2_mixer_flags += {'mpg123': 'enabled'}
    sdl2_mixer_defines += '-DAUDIO_WITH_MP3_SUPPORT'
else
    mpg123_dep = cpp.find_library('mpg123', required: false)
    if mpg123_dep.found()
        sdl2_mixer_flags += {'mpg123': 'enabled'}
        sdl2_mixer_defines += '-DAUDIO_WITH_MP3_SUPPORT'

        meson.override_dependency('mpg123', mpg123_dep)
    endif
endif

sdl2_mixer_dep = dependency(
    'sdl2_mixer',
    'SDL2_mixer',
    allow_fallback: not only_allow_native_libs,
    required: true,
    default_options: sdl2_mixer_flags,
)
graphics_lib += {
    'deps': [graphics_lib.get('deps'), sdl2_mixer_dep],
    'compile_args': [graphics_lib.get('compile_args'), sdl2_mixer_defines],
}

fmt_use_header_only = false

if (
    meson.is_cross_build()
    and (host_machine.system() == 'switch' or host_machine.system() == '3ds')
)
    fmt_use_header_only = true
    # clang with libc++ creates some really long and confusing linker errors, so just use the header only library
elif cpp.get_id() == 'clang' and build_with_libcpp
    fmt_use_header_only = true
endif

if fmt_use_header_only
    fmt_header_only_dep = dependency(
        'fmt_header_only',
        required: true,
        allow_fallback: true,
    )
    meson.override_dependency('fmt', fmt_header_only_dep)
    fmt_dep = fmt_header_only_dep
else
    fmt_dep = dependency('fmt', required: true, allow_fallback: true)
endif

core_lib += {'deps': [core_lib.get('deps'), fmt_dep]}

spdlog_dep = dependency(
    'spdlog',
    required: true,
    default_options: {'tests': 'disabled'},
)
graphics_lib += {
    'deps': [graphics_lib.get('deps'), spdlog_dep],
}

if (meson.is_cross_build() and host_machine.system() == '3ds')
    graphics_lib += {
        'compile_args': [graphics_lib.get('compile_args'), '-DSPDLOG_NO_TLS'],
    }
endif

nlohmann_json_dep = dependency(
    'nlohmann_json',
    required: true,
)
core_lib += {'deps': [core_lib.get('deps'), nlohmann_json_dep]}

# check std::expected support
have_std_expected = cpp.compiles(
    '''
#include <expected>
int main() {
    using T = std::expected<int, char>;
    return 0;
}
''',
    args: core_lib.get('compile_args'),
)

if not have_std_expected
    message('Compiler doesn\'t support std::expected, using fallback')

    tl_exp_dep = dependency('tl-expected', required: true)
    core_lib += {
        'compile_args': [core_lib.get('compile_args'), '-D_USE_TL_EXPECTED'],
        'deps': [core_lib.get('deps'), tl_exp_dep],
    }
else
    message('Compiler support std::expected, using that')
endif

# check std::optional support
have_std_optional = cpp.compiles(
    '''
#include <optional>
int main() {
    using T = std::optional<int>;
    return 0;
}
''',
    args: core_lib.get('compile_args'),
)

if not have_std_optional
    message('Compiler doesn\'t support std::optional, using fallback')
    tl_opt_dep = dependency('tl-optional', required: true)
    core_lib += {
        'compile_args': [core_lib.get('compile_args'), '-D_USE_TL_OPTIONAL'],
        'deps': [core_lib.get('deps'), tl_opt_dep],
    }
else
    message('Compiler support std::optional, using that')
endif

magic_enum_dep = dependency(
    'magic_enum',
    required: true,
    default_options: {'test': false},
)
core_lib += {'deps': [core_lib.get('deps'), magic_enum_dep]}

argparse_dep = dependency('argparse', required: true)
core_lib += {'deps': [core_lib.get('deps'), argparse_dep]}

online_multiplayer_supported = true

if (
    meson.is_cross_build()
    and (host_machine.system() == 'switch' or host_machine.system() == '3ds')
)
    online_multiplayer_supported = false

    # TODO: use libcurl and
    # https://github.com/uctakeoff/uc-curl
    # or https://github.com/JosephP91/curlcpp

    core_lib += {
        'compile_args': [
            core_lib.get('compile_args'),
            '-D_ONLINE_MULTIPLAYER_NOT_SUPPORTED',
        ],
    }
else
    cpp_httlib_dep = dependency(
        'cpp-httplib',
        required: true,
        default_options: {
            'cpp-httplib_openssl': 'enabled',
            'cpp-httplib_zlib': 'enabled',
        },
    )
    core_lib += {'deps': [core_lib.get('deps'), cpp_httlib_dep]}
endif

utf8cpp_dep = dependency(
    'utf8cpp',
    required: true,
    version: '>=4.0.0',
)
core_lib += {'deps': [core_lib.get('deps'), utf8cpp_dep]}

build_installer = get_option('build_installer')

is_flatpak_build = false

# some sanity checks for the installer
if build_installer
    if get_option('buildtype') != 'release'
        error(
            'buildtype needs to be \'release\', when building the installer, but was: '
            + get_option('buildtype'),
        )
    endif

    if host_machine.system() == 'linux'
        if get_option('prefix') == '/app'
            is_flatpak_build = true
        else
            error(
                'only support flatpak builds, when  building the installer for linux',
            )
        endif
    elif host_machine.system() == 'windows'
        message('Adding a windows installer target: \'windows_installer\'')
    else
        error(
            'unsuported system for building the installer: '
            + host_machine.system(),
        )

    endif

    core_lib += {
        'compile_args': [
            core_lib.get('compile_args'),
            '-DBUILD_INSTALLER',
        ],
    }

endif

if is_flatpak_build
    app_name = 'com.github.mgerhold.OOPetris'
    core_lib += {
        'compile_args': [
            core_lib.get('compile_args'),
            '-DFLATPAK_BUILD'
        ],
    }
endif

have_file_dialogs = false
have_discord_sdk = false

nfde_dep = dependency(
    'nativefiledialog-extended',
    required: not meson.is_cross_build(),
    default_options: {
        'xdg-desktop-portal': is_flatpak_build ? 'enabled' : 'auto',
    },
)
if nfde_dep.found()
    have_file_dialogs = true
    graphics_lib += {
        'compile_args': [
            graphics_lib.get('compile_args'),
            '-D_HAVE_FILE_DIALOGS',
        ],
        'deps': [graphics_lib.get('deps'), nfde_dep],
    }
endif

discord_sdk_dep = dependency(
    'discord-game-sdk',
    required: not meson.is_cross_build(),
)
if discord_sdk_dep.found()
    have_discord_sdk = true
    graphics_lib += {
        'compile_args': [
            graphics_lib.get('compile_args'),
            '-D_HAVE_DISCORD_SDK',
        ],
        'deps': [graphics_lib.get('deps'), discord_sdk_dep],
    }
endif
