
if meson.is_cross_build()
    error('Tests are not supported for cross builds (atm)')
endif

test_deps = deps
test_compile_args = compile_args

test_src = []
test_inc_dirs = []

test_deps += dependency('gtest')


test_src += files('entry.cpp')

subdir('tests')
subdir('utils')

test_inc_dirs += include_directories('.')

all_tests = executable(
    'all_tests',
    test_src,
    include_directories: test_inc_dirs,
    dependencies: test_deps,
    cpp_args: test_compile_args,
    override_options: {
        'warning_level': '3',
        'werror': true,
    },
)

test(
    'all tests',
    all_tests,
    protocol: 'gtest',
    workdir: meson.project_source_root() / 'tests' / 'files',
)
