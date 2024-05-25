include_dir = get_option('prefix') / get_option('includedir')

core_lib += {
    'inc_dirs': [core_lib.get('inc_dirs'), include_directories('.')],
}

subdir('core')

recordings_lib += {
    'inc_dirs': [recordings_lib.get('inc_dirs'), include_directories('.')],
}

subdir('recordings')
