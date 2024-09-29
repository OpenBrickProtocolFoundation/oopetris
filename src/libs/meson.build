include_dir = get_option('prefix') / get_option('includedir')

core_lib += {
    'inc_dirs': [core_lib.get('inc_dirs'), include_directories('.')],
}

pkg_cpp_compiler = cpp.get_id()
pkg_cpp_stdlib = 'unknown'
if cpp.get_id() == 'clang'
    pkg_cpp_stdlib = build_with_libcpp ? 'libc++' : 'libstdc++'
endif

subdir('core')

recordings_lib += {
    'inc_dirs': [recordings_lib.get('inc_dirs'), include_directories('.')],
}

subdir('recordings')

subdir('plugins')
