subdir('libs')

if build_application

    graphics_src_files = []

    subdir('game')
    subdir('graphics')
    subdir('helper')
    subdir('input')
    subdir('manager')
    subdir('scenes')
    subdir('ui')

    if online_multiplayer_supported
        subdir('lobby')
    endif

    if have_discord_sdk
        subdir('discord')
    endif

    graphics_lib += {
        'deps': [graphics_lib.get('deps'), liboopetris_recordings_dep],
        'inc_dirs': [graphics_lib.get('inc_dirs'), include_directories('.')],
        'compile_args' : [graphics_lib.get('compile_args'),
        '-DOOPETRIS_LIBRARY_GRAPHICS_TYPE='+
            (get_option('default_library') == 'static' ? '1' : '0')]
    }

    liboopetris_graphics = library(
        'oopetris_graphics',
        graphics_src_files,
        include_directories: graphics_lib.get('inc_dirs'),
        dependencies: graphics_lib.get('deps'),
        cpp_args: graphics_lib.get('compile_args'),
        cpp_shared_args: ['-DOOPETRIS_LIBRARY_GRAPHICS_EXPORT'],
        override_options: {
            'warning_level': '3',
            'werror': true,
        },
        version: meson.project_version(),
        install: true,
    )

    liboopetris_graphics_dep = declare_dependency(
        link_with: liboopetris_graphics,
        include_directories: graphics_lib.get('inc_dirs'),
        compile_args: graphics_lib.get('compile_args'),
        dependencies: graphics_lib.get('deps'),
        version: meson.project_version(),
    )
    meson.override_dependency('liboopetris-graphics', liboopetris_graphics_dep)

    # setting this to strings, so += {...} gets detected as an error, if it is done after that
    graphics_lib = 'undefined'

endif
