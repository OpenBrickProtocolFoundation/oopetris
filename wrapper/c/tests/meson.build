test_deps = []

test_src = [
    files(
        'get_recording_information.c',
        'information.c',
        'is_recording_file.c',
    ),
]

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
    protocol: 'tap', ## use 'exitcode' to get better messages, of what exactly the failure was
    workdir: meson.project_source_root() / 'tests' / 'files',
    args: ['--tap'], ## remove '--tap' and add '--verbose', '--full-stats' to get better failure messages
)
