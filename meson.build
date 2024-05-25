project(
    'oopetris',
    'cpp',
    'c',
    meson_version: '>=1.3.0',
    default_options: {
        'buildtype': 'debug',
        'optimization': '3',
        'strip': true,
        'cpp_std': ['c++23', 'c++latest', 'vc++latest', 'c++20'],
        'b_ndebug': 'if-release',

    },
    version: '0.5.6',
)

subdir('tools/options')

subdir('tools/dependencies')

subdir('src')

subdir('tools/install')

if get_option('tests')
    subdir('tests')
endif
