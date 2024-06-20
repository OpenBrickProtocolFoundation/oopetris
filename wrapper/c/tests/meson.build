test_deps = []

test_src = [files('main.c')]

test_deps += dependency('criterion')

tests = executable(
    'tests',
    test_src,
    dependencies: [test_deps, liboopetris_c_wrapper_dep],
    override_options: {
        'warning_level': '3',
        'werror': true,
        'b_coverage': false,
    },
)

test(
    'tests',
    tests,
    protocol: 'tap',
    workdir: meson.project_source_root() / 'tests' / 'files',
    args: ['--tap'],
)
