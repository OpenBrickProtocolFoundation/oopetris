if build_application

    main_files = []

    subdir('game')

    if meson.is_cross_build()

        if host_machine.system() == 'android'

            library(
                'oopetris',
                main_files,
                dependencies: [liboopetris_graphics_dep, graphic_application_deps],
                override_options: {
                    'warning_level': '3',
                    'werror': true,
                },
            )

        elif host_machine.system() == 'switch'
            switch_options = [
                app_name,
                main_files,
                [liboopetris_graphics_dep, graphic_application_deps],
            ]
            subdir('platforms/switch')
        elif host_machine.system() == '3ds'
            _3ds_options = [
                app_name,
                main_files,
                [liboopetris_graphics_dep, graphic_application_deps],
            ]
            subdir('platforms/3ds')
        elif host_machine.system() == 'emscripten'
            emscripten_link_args = []

            APP_ROMFS = meson.get_external_property('APP_ROMFS', '')

            if APP_ROMFS != ''

                fs = import('fs')

                if not fs.is_absolute(APP_ROMFS)
                    APP_ROMFS = meson.project_source_root() / APP_ROMFS
                endif

                if not fs.exists(APP_ROMFS)
                    error(
                        'APP_ROMFS should exist, but doesn\'t: \''
                        + APP_ROMFS
                        + '\'',
                    )
                endif

                if not APP_ROMFS.endswith('/')
                    APP_ROMFS = APP_ROMFS + '/'
                endif

                emscripten_link_args += ['--preload-file', APP_ROMFS + '@/assets/']

            endif

            emscripten_name = 'oopetris'
            emscripten_deps = [liboopetris_graphics_dep, graphic_application_deps]

            emscripten_options = {
                'warning_level': '3',
                'werror': true,
            }

            oopetris_js = executable(
                emscripten_name,
                main_files,
                dependencies: emscripten_deps,
                link_args: emscripten_link_args,
                override_options: emscripten_options,
                install: true,
                name_suffix: 'js',
            )

            oopetris_html = executable(
                emscripten_name,
                objects: oopetris_js.extract_all_objects(recursive: true),
                dependencies: emscripten_deps,
                link_args: emscripten_link_args,
                override_options: emscripten_options,
                install: true,
                name_suffix: 'html',
                link_depends: [oopetris_js],
            )

            python3 = find_program('python3', required: false)

            if python3.found()

                server_py = files(
                    meson.project_source_root() / 'platforms' / 'emscripten' / 'server.py',
                )

                dest_path = meson.project_build_root() / 'src' / 'executables'

                run_target(
                    'serve_emscripten',
                    command: [python3, server_py, '-d', dest_path],
                    depends: [oopetris_js, oopetris_html],
                )

            endif

            wasm_validate_exe = find_program('wasm-validate', required: false)

            if wasm_validate_exe.found()

                wasm_file = meson.project_build_root() / 'src' / 'executables' / 'oopetris.wasm'

                test(
                    'validate wasm file',
                    wasm_validate_exe,
                    args: ['--enable-threads', wasm_file],
                    depends: [oopetris_js, oopetris_html],

                )
            endif

        else
            error('Unhandled cross built system: ' + host_machine.system())
        endif
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

        recordings_main_files = []

        subdir('utility')

        oopetris_recordings_utility_exe = executable(
            'oopetris_recordings_utility',
            recordings_main_files,
            dependencies: [liboopetris_recordings_dep, recordings_application_deps],
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

                nsis_script = files(
                    meson.project_source_root() / 'tools' / 'installer' / 'setup.nsi',
                )

                run_target(
                    'windows_installer',
                    command: [
                        makensis,
                        '-DVERSION=' + meson.project_version(),
                        '-DNAME=' + oopetris_name,
                        '-DAUTHOR=' + oopetris_author,
                        '-DPROJECT_SOURCE_DIR='
                        + meson.project_source_root(),
                        '-DPROJECT_BUILD_DIR=' + meson.project_build_root(),
                        '-DDYNAMIC_LIBRARIES_DIR='
                        + (
                            meson.project_build_root() / 'dynamic_libraries' / get_option('bindir')
                        ),
                        nsis_script,
                    ],
                    depends: [oopetris_exe, oopetris_recordings_utility_exe],
                )

            endif
        endif

    endif

endif
