project(
    'discord-game-sdk',
    'cpp',
    'c',
    version: '3.2.1',
    meson_version: '>=1.4.0',
    default_options: {
        'cpp_std': ['c++17'],
    },
)

subdir('cpp')

if get_option('examples')
    subdir('examples/cpp')
endif
