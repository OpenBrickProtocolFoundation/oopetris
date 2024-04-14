
core_lib = {
    'src_files': [],
    'inc_dirs': [],
    'compile_args': ['-DOOPETRIS_VERSION=' + meson.project_version()],
    'deps': [],
}

recordings_lib = {
    'src_files': [],
    'inc_dirs': [],
    'compile_args': [],
    'deps': [],
}

graphics_lib = {
    'src_files': [],
    'inc_dirs': [],
    'compile_args': [],
    'deps': [],
}

global_deps = []
graphic_application_deps = []


cpp = meson.get_compiler('cpp')

build_with_libcpp = false

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
        add_global_link_arguments('-stdlib=libc++', language: ['cpp'])
        add_global_arguments('-stdlib=libc++', language: ['cpp'])
        global_deps += [cpp.find_library('c++'), cpp.find_library('c++abi')]
    endif

endif

if (
    get_option('buildtype') == 'debug'
    or get_option('buildtype') == 'debugoptimized'
)
#TODO: replace with ! NDEBUG in cpp files and remove this
    core_lib += {
        'compile_args': [core_lib.get('compile_args'), '-DDEBUG_BUILD'],
    }
endif


