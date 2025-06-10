oopetris_author = 'Coder2k'
oopetris_name = 'OOPetris'

core_lib = {
    'inc_dirs': [],
    'compile_args': ['-DOOPETRIS_VERSION=' + meson.project_version()],
    'deps': [],
}

recordings_lib = {
    'inc_dirs': [],
    'compile_args': [],
    'deps': [],
}

graphics_lib = {
    'inc_dirs': [],
    'compile_args': [
        '-DOOPETRIS_VERSION=' + meson.project_version(),
        '-DOOPETRIS_NAME=' + oopetris_name,
        '-DOOPETRIS_AUTHOR=' + oopetris_author,
    ],
    'deps': [],
}

# check if running in CI, than we only support certain versions, this helps in checking if the Ci installs the correct tool version in e.g. msys2
if get_option('run_in_ci')

    c = meson.get_compiler('c')

    system_name = host_machine.system()

    if meson.is_cross_build()
        system_name = 'cross_' + host_machine.system()

    endif

    # Detect if compiling in an MSYS2-based MinGW/Clang environment
    host_is_msys2 = host_machine.system() == 'windows' and c.get_argument_syntax() == 'gcc'
    if host_is_msys2
        system_name = 'msys2'
    endif

    clang_20_compiler = {
        'id': 'clang',
        'checks': ['>=20', '<21'],
    }

    gcc_14_compiler = {
        'id': 'gcc',
        'checks': ['>=14', '<15'],
    }

    msvc_compiler_current = {
        'id': 'msvc',
        'checks': ['>=19.43', '<20'],
    }

    emscripten_compiler_current = {
        'id': 'emscripten',
        'checks': ['>=4', '<5'],
    }

    ci_config = {
        'cross_android': {
            'compilers': [clang_20_compiler],
        },
        'cross_3ds': {
            'compilers': [gcc_14_compiler],
        },
        'cross_switch': {
            'compilers': [gcc_14_compiler],
        },
        'cross_emscripten': {
            'compilers': [emscripten_compiler_current],
        },
        'windows': {
            'compilers': [msvc_compiler_current],
        },
        'msys2': {
            'compilers': [clang_20_compiler, gcc_14_compiler],
        },
        'linux': {
            'compilers': [clang_20_compiler, gcc_14_compiler],
        },
        'darwin': {
            'compilers': [clang_20_compiler],
        },
    }

    if not ci_config.has_key(system_name)
        error('CI config doesn\'t know what to expect from the system "' + system_name + '"')
    endif

    config_for_system = ci_config.get(system_name)

    compilers_for_system = config_for_system.get('compilers', [])

    if compilers_for_system.length() == 0
        error('Expected the CI config to have at least one compiler')
    endif

    found_compiler_for_ci_config = false

    foreach compiler_for_system : compilers_for_system
        compiler_for_system_name = compiler_for_system.get('id')
        version_checks_for_compiler = compiler_for_system.get('checks')

        if c.get_id() != compiler_for_system_name
            continue
        endif

        found_compiler_for_ci_config = true

        if version_checks_for_compiler.length() == 0
            error('Expected the version checks for a compiler to not be empty')
        endif

        compiler_version = c.version()

        foreach version_check_for_compiler : version_checks_for_compiler

            if not compiler_version.version_compare(version_check_for_compiler)
                error(
                    'The compiler "' + c.get_id() + '" with version "' + compiler_version + '" doesn\'t meet the version check "' + version_check_for_compiler + '"'
                )
            endif

        endforeach


    endforeach

    if not found_compiler_for_ci_config
        error(
            'Couldn\'t find a valid compiler for the CI config, compiler was: "' + c.get_id() + '"'
        )
    endif

endif



if meson.is_cross_build() and host_machine.system() == 'serenity'
    temp = core_lib.get('compile_args')
    temp += ['-D__SERENITY__', '-DINSTALL_FILES', '-DINSTALL_LOCATION=' + get_option('prefix')]
    core_lib += {
        'compile_args': temp,
    }
    temp = 0
elif meson.is_cross_build() and host_machine.system() == 'android'
    cpp = meson.get_compiler('cpp')
    target_android_api = '35'

    android_api = cpp.get_define(
        '__ANDROID_API__',
        prefix: '#include <features.h>',
    ).strip('"')
    assert(android_api != '', 'failed to get android_api version')

    android_compile_args = []
    android_link_args = []

    # APP_SUPPORT_FLEXIBLE_PAGE_SIZES
    android_link_args += '-Wl,-z,max-page-size=16384'

    if android_api != target_android_api
        android_compile_args += ('-D__ANDROID_API__=' + target_android_api)
    endif

    android_compile_args += [
        '-D__ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__',
        '-Werror=unguarded-availability',
    ]

    if not meson.is_subproject()
        add_global_link_arguments(
            android_link_args,
            language: ['cpp'],
        )
        add_global_arguments(
            android_compile_args,
            language: ['cpp'],
        )
    else
        add_project_link_arguments(
            android_link_args,
            language: ['cpp'],
        )
        add_project_arguments(
            android_compile_args,
            language: ['cpp'],
        )

    endif
endif

cpp = meson.get_compiler('cpp')

build_with_libcpp = false

allow_tl_expected_fallback = false

if cpp.get_id() == 'gcc'
    add_project_arguments(
        '-Wold-style-cast',
        language: ['cpp'],
    )
elif cpp.get_id() == 'clang'

    build_with_libcpp_option = get_option('clang_libcpp')

    build_with_libcpp = build_with_libcpp_option.enabled()

    if build_with_libcpp_option.auto()
        if host_machine.system() == 'darwin'
            build_with_libcpp = true
        else

            cpp_lib = cpp.find_library(
                'c++',
                required: false,
            )
            cppabi_lib = cpp.find_library(
                'c++abi',
                required: false,
            )

            if cpp_lib.found() and cppabi_lib.found()
                build_with_libcpp = true
            else
                build_with_libcpp = false
            endif
        endif
    endif

    if build_with_libcpp

        temp = core_lib.get('compile_args')
        temp += '-stdlib=libc++'

        core_lib += {
            'compile_args': temp,
            'deps': [core_lib.get('deps'), cpp.find_library('c++'), cpp.find_library('c++abi')],
        }
        temp = 0

        if not meson.is_subproject()
            add_global_link_arguments(
                '-stdlib=libc++',
                language: ['cpp'],
            )
            add_global_arguments(
                '-stdlib=libc++',
                language: ['cpp'],
            )
        else
            add_project_link_arguments(
                '-stdlib=libc++',
                language: ['cpp'],
            )
            add_project_arguments(
                '-stdlib=libc++',
                language: ['cpp'],
            )

        endif

    else
        # TODO: once clang with libstdc++ (gcc c++ stdlib) supports std::expected, remove this special behaviour
        allow_tl_expected_fallback = true
    endif
elif cpp.get_id() == 'emscripten'
    allow_tl_expected_fallback = true
endif

build_application = true

## only build, if we are at the root, not if this is used as subproject in e.g. wrap files
if meson.is_subproject() or get_option('only_build_libs')
    build_application = false

    if get_option('build_installer') and get_option('only_build_libs')
        error('Can\'t build installer when \'only_build_libs\' is enabled')
    endif
endif
