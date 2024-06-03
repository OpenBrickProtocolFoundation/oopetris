oopetris_author = 'Coder2k'
oopetris_name = 'OOPetris'

core_lib = {
    'inc_dirs': [],
    'compile_args': [
        '-DOOPETRIS_VERSION=' + meson.project_version(),
    ],
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

if meson.is_cross_build() and host_machine.system() == 'serenity'
    core_lib += {
        'compile_args': [core_lib.get('compile_args'), '-D__SERENITY__'],
    }
endif


cpp = meson.get_compiler('cpp')

build_with_libcpp = false

allow_tl_expected_fallback = false

if cpp.get_id() == 'gcc'
    add_project_arguments('-Wold-style-cast', language: ['cpp'])
elif cpp.get_id() == 'clang'

    build_with_libcpp_option = get_option('clang_libcpp')

    build_with_libcpp = build_with_libcpp_option.enabled()

    if build_with_libcpp_option.auto()
        if host_machine.system() == 'darwin'
            build_with_libcpp = true
        else

            cpp_lib = cpp.find_library('c++', required: false)
            cppabi_lib = cpp.find_library('c++abi', required: false)

            if cpp_lib.found() and cppabi_lib.found()
                build_with_libcpp = true
            else
                build_with_libcpp = false
            endif
        endif
    endif

    if build_with_libcpp

        core_lib += {
            'compile_args': [core_lib.get('compile_args'), '-stdlib=libc++'],
            'deps': [
                core_lib.get('deps'),
                cpp.find_library('c++'),
                cpp.find_library('c++abi'),
            ],
        }

        if not meson.is_subproject()
            add_global_link_arguments('-stdlib=libc++', language: ['cpp'])
            add_global_arguments('-stdlib=libc++', language: ['cpp'])
        else
            add_project_link_arguments('-stdlib=libc++', language: ['cpp'])
            add_project_arguments('-stdlib=libc++', language: ['cpp'])

        endif

    else
        # TODO: once clang with libstdc++ (gcc c++ stdlib) supports std::expectedt, remove this special behaviour
        allow_tl_expected_fallback = true
    endif

endif

build_application = true

## only build, if we are at the root, not if this is used as subproject in e.g. wrap files
if meson.is_subproject() or get_option('only_build_libs')
    build_application = false

    if get_option('build_installer') and get_option('only_build_libs')
        error('Can\'t build installer when \'only_build_libs\' is enabled')
    endif
endif
