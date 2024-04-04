
if meson.is_cross_build()
    error('Tests are not supported for cross builds (atm)')
endif

test_deps = []

core_test_src = []
test_src = []
test_inc_dirs = []

test_deps += dependency('gtest')
test_deps += dependency('gmock')

core_test_src += files('core_entry.cpp')

subdir('core')
subdir('utils')

test_inc_dirs += include_directories('.')

core_tests = executable(
    'core_tests',
    test_src,
    core_test_src,
    include_directories: test_inc_dirs,
    dependencies: [test_deps, liboopetris_core_dep],
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
)

test(
    'core_tests',
    core_tests,
    protocol: 'gtest',
    workdir: meson.project_source_root() / 'tests' / 'files',
)
