project(
    'c-embed',
    'c',
    meson_version: '>=1.12.0',
    version: '0.0.1',
    license: 'MIT',
    default_options: {
        'optimization': '3',
        'warning_level': '2',
        'werror': true,
        'c_std': ['c11'],
        'b_ndebug': 'if-release',
    },
)

inc_dirs = include_directories('.')



c_embed_exe = executable(
    'c-embed',
    files('c-embed.c'),
    include_directories: inc_dirs,
    native: true,
)

meson.override_find_program(
    'c-embed',
    c_embed_exe,
    native: true,
)


c_embed_dep = declare_dependency(
    include_directories: inc_dirs,
    version: meson.project_version(),
)

meson.override_dependency('c-embed', c_embed_dep)
