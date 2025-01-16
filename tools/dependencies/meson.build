only_allow_native_libs = false

if meson.is_cross_build()
    if host_machine.system() == 'switch' or host_machine.system() == '3ds'
        # we do not link to code that was compiled with gcc 10.1 / gcc 7.1, the code we link with is all compiled with gcc 13.2

        temp = core_lib.get('compile_args')
        temp += '-Wno-psabi'

        core_lib += {
            'compile_args': temp,
        }
        temp = 0

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
    elif host_machine.system() == 'android'

        flac_dep = dependency(
            'flac',
            allow_fallback: false,
            required: true,
        )

        meson.override_dependency('libflac', flac_dep)

    elif host_machine.system() == 'emscripten'

        # check if the command line flags are supported and a simple example compiles
        can_compile = cpp.compiles(
            '''
        int main() {
            return 0;
        }
        ''',
        )

        if not can_compile
            error('Not all Emscripten flags are supported: see logs')
        endif

        only_allow_native_libs = true

        ## map native libraries to dependencies
        map_native_dependencies = [
            ['SDL2-mt', 'SDL2'],
            ['SDL2_ttf'],
            ['mpg123'],
            ['SDL2_mixer-mp3-mt', 'SDL2_mixer'],
            ['SDL2_image-png-svg-mt', 'SDL2_image'],
            ['icu_common-mt', 'icu-uc'],
        ]
        foreach native_dependency_tuple : map_native_dependencies
            native_dep_lib_name = native_dependency_tuple[0]

            native_dep_name = native_dependency_tuple.length() == 2 ? native_dependency_tuple[1] : native_dep_lib_name

            native_dep = cpp.find_library(native_dep_lib_name, required: true)

            if native_dep_name == 'SDL2'

                major_version = cpp.get_define(
                    'SDL_MAJOR_VERSION',
                    prefix: '#include <SDL2/SDL_version.h>',
                ).strip('"')
                assert(
                    major_version != '',
                    'failed to get major_version from SDL_version.h',
                )

                minor_version = cpp.get_define(
                    'SDL_MINOR_VERSION',
                    prefix: '#include <SDL2/SDL_version.h>',
                ).strip('"')
                assert(
                    minor_version != '',
                    'failed to get minor_version from SDL_version.h',
                )

                patch_version = cpp.get_define(
                    'SDL_PATCHLEVEL',
                    prefix: '#include <SDL2/SDL_version.h>',
                ).strip('"')
                assert(
                    patch_version != '',
                    'failed to get patch_version from SDL_version.h',
                )

                native_dep_with_version = declare_dependency(
                    dependencies: native_dep,
                    version: major_version
                    + '.'
                    + minor_version
                    + '.'
                    + patch_version,
                )
                meson.override_dependency(native_dep_name, native_dep_with_version)
            else
                meson.override_dependency(native_dep_name, native_dep)

            endif

        endforeach

        native_deps = []

        native_dep_names = [
            'embind',
            'embind-rtti',
            'freetype',
            'harfbuzz-mt',
            'z',
            'mpg123',
            'png-mt',
            'fetch-mt',
        ]

        foreach native_dep_name : native_dep_names
            native_deps += cpp.find_library(native_dep_name, required: true)
        endforeach

        graphics_lib += {
            'deps': [graphics_lib.get('deps'), native_deps],
        }

    else
        error('Unhandled cross built system: ' + host_machine.system())
    endif
endif

fmt_use_header_only = false

if (
    meson.is_cross_build()
    and (host_machine.system() == 'switch' or host_machine.system() == '3ds')
)
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
    if not allow_tl_expected_fallback
        error('Compiler doesn\'t support std::expected')
    endif

    message('Compiler doesn\'t support std::expected, using fallback')

    tl_exp_dep = dependency('tl-expected', required: true)

    temp = core_lib.get('compile_args')
    temp += '-D_USE_TL_EXPECTED'
    core_lib += {
        'compile_args': temp,
        'deps': [core_lib.get('deps'), tl_exp_dep],
    }
    temp = 0
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
    error('Compiler doesn\'t support std::optional')
endif

magic_enum_dep = dependency(
    'magic_enum',
    required: true,
    default_options: {'test': false, 'hash': true},
)
core_lib += {'deps': [core_lib.get('deps'), magic_enum_dep]}

utf8cpp_dep = dependency(
    'utf8cpp',
    required: true,
    version: '>=4.0.0',
)
graphics_lib += {'deps': [graphics_lib.get('deps'), utf8cpp_dep]}

icu_dep = dependency(
    'icu-uc',
    required: false,
)

if icu_dep.found()
    graphics_lib += {
        'compile_args': [
            graphics_lib.get('compile_args'),
            '-D_HAVE_ICU_DEP',
        ],
        'deps': [graphics_lib.get('deps'), icu_dep],
    }
endif

is_flatpak_build = false

if build_application

    graphic_application_deps = []
    recordings_application_deps = []

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

    cpp_httlib_dep = dependency(
        'cpp-httplib',
        required: false,
        allow_fallback: true,
        default_options: {
            'cpp-httplib_openssl': 'enabled',
            'cpp-httplib_zlib': 'enabled',
        },
    )

    online_multiplayer_system = 'httplib'

    if cpp_httlib_dep.found()

        graphics_lib += {
            'deps': [graphics_lib.get('deps'), cpp_httlib_dep],
            'compile_args': [
                graphics_lib.get('compile_args'),
                '-D_OOPETRIS_ONLINE_SYSTEM=0',
            ],
        }

    elif meson.is_cross_build() and host_machine.system() == 'emscripten'
        online_multiplayer_system = 'web'

        graphics_lib += {
            'compile_args': [
                graphics_lib.get('compile_args'),
                '-D_OOPETRIS_ONLINE_SYSTEM=1',
            ],
        }
    else

        online_multiplayer_system = 'curl'

        curl_cpp_wrapper = dependency(
            'cpr',
            required: true,
            default_options: {
                'tests': 'disabled',
            },
        )

        graphics_lib += {
            'deps': [graphics_lib.get('deps'), curl_cpp_wrapper],
            'compile_args': [
                graphics_lib.get('compile_args'),
                '-D_OOPETRIS_ONLINE_SYSTEM=2',
            ],
        }

    endif

    if host_machine.system() == 'linux' or (meson.is_cross_build() and host_machine.system() == 'android')
        keyutils_dep = dependency('keyutils', required: true, allow_fallback: true)
        graphics_lib += {'deps': [graphics_lib.get('deps'), keyutils_dep]}
    elif host_machine.system() == 'windows'
        c = meson.get_compiler('c')

        ncrypt_dep = c.find_library('ncrypt')
        graphics_lib += {'deps': [graphics_lib.get('deps'), ncrypt_dep]}
    endif

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
            # TODO: create a proper installer for macOS : https://mesonbuild.com/Creating-OSX-packages.html
            error(
                'unsuported system for building the installer: '
                + host_machine.system(),
            )

        endif

        temp = core_lib.get('compile_args')
        temp += '-DBUILD_INSTALLER'
        core_lib += {'compile_args': temp}
        temp = 0

    endif

    if is_flatpak_build
        app_name = 'io.github.openbrickprotocolfoundation.oopetris'

        temp = core_lib.get('compile_args')
        temp += '-DFLATPAK_BUILD'
        core_lib += {'compile_args': temp}
        temp = 0
    endif

    have_file_dialogs = false
    have_discord_sdk = false

    nfde_dep = dependency(
        'nativefiledialog-extended',
        required: not meson.is_cross_build(),
        allow_fallback: true,
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

    c = meson.get_compiler('c')

    discord_dep_required = not meson.is_cross_build() and get_option('build_installer')

    if host_machine.system() == 'linux' and host_machine.cpu_family() == 'aarch64'
        # not supported on aarch64 linux :(
        discord_dep_required = false
    endif

    discord_sdk_dep = dependency(
        'discord-game-sdk',
        allow_fallback: true,
        required: discord_dep_required,
        # only with msvc we need a static library, all others work without adding __declspec() everywhere
        static: c.get_id() == 'msvc',
        default_options: c.get_id() != 'msvc' ? {} : {'default_library': 'static'},
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

    argparse_dep = dependency('argparse', required: true)

    graphic_application_deps += argparse_dep
    recordings_application_deps += argparse_dep
endif
