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

oopetris_author = 'Coder2k'
oopetris_name = 'OOPetris'

subdir('tools/options')

subdir('tools/dependencies')

subdir('src')

subdir('tools/install')

if meson.is_cross_build() and host_machine.system() == 'android'

    library(
        'oopetris',
        main_files,
        dependencies: [liboopetris_graphics_dep, graphic_application_deps],
        override_options: {
            'warning_level': '3',
            'werror': true,
        },
    )

elif meson.is_cross_build() and host_machine.system() == 'switch'
    switch_options = [
        app_name,
        main_files,
        [liboopetris_graphics_dep, graphic_application_deps],
    ]
    subdir('platforms/switch')
elif meson.is_cross_build() and host_machine.system() == '3ds'
    _3ds_options = [
        app_name,
        main_files,
        [liboopetris_graphics_dep, graphic_application_deps],
    ]
    subdir('platforms/3ds')
else

    if host_machine.system() == 'windows'
        subdir('platforms/windows')
    endif

    oopetris_exe = executable(
        'oopetris',
        main_files,
        dependencies: [liboopetris_graphics_dep, graphic_application_deps],
        override_options: {
            'warning_level': '3',
            'werror': true,
        },
        install: true,
        win_subsystem: 'windows',
    )

    oopetris_recordings_utility_exe = executable(
        'oopetris_recordings_utility',
        recordings_main_files,
        dependencies: liboopetris_recordings_dep,
        override_options: {
            'warning_level': '3',
            'werror': true,
        },
        install: true,
        win_subsystem: 'console',
    )

    if build_installer
        if host_machine.system() == 'windows'

            makensis = find_program('makensis')

            nsis_script = meson.project_source_root() / 'tools' / 'installer' / 'setup.nsi'

            run_target(
                'windows_installer',
                command: [
                    makensis,
                    '-DVERSION=' + meson.project_version(),
                    '-DNAME=' + oopetris_name,
                    '-DAUTHOR=' + oopetris_author,
                    '-DPROJECT_SOURCE_DIR=' + meson.project_source_root(),
                    '-DPROJECT_BUILD_DIR=' + meson.project_build_root(),
                    nsis_script,
                ],
                depends: [oopetris_exe, oopetris_recordings_utility_exe],
            )

        endif
    endif

endif

if get_option('tests')
    subdir('tests')
endif
