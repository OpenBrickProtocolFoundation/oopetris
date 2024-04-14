
## TODO: only install needed ones, since sometimes we only need e.g. flacs or mp3 and no icons etc.
## install assets
install_subdir(
    meson.project_source_root() / 'assets',
    install_dir: 'share/oopetris',
    exclude_files: ['oopetris.desktop.in', 'OOPetris.svg', 'recordings.magic'],
    exclude_directories: ['icon'],
)

app_name = 'oopetris'
if is_flatpak_build
    app_name = 'com.github.mgerhold.OOPetris'
endif

conf = configuration_data()
conf.set('APP_NAME', app_name)

datadir = get_option('prefix') / get_option('datadir')


if host_machine.system() == 'linux'

    fs = import('fs')

    magic_dir = datadir / 'misc'
    magic_file = magic_dir / 'magic'
    oopetris_magic_file = (
        meson.project_source_root() / 'assets' / 'recordings.magic'
    )

    if fs.exists(magic_file)

        cat_prog = find_program('cat')

        custom_target(
            'magic_file_append',
            command: [cat_prog, '@INPUT@'],
            capture: true,
            input: [oopetris_magic_file, magic_file],
            output: 'magic',
            install_dir: magic_dir,
        )

    else

        install_data(
            oopetris_magic_file,
            install_dir: magic_dir,
            rename: ['magic'],
        )

    endif
endif

configure_file(
    input: meson.project_source_root() / 'assets/oopetris.desktop.in',
    output: app_name + '.desktop',
    configuration: conf,
    install: true,
    install_dir: datadir / 'applications',
)

logos = [
    '24x24.png',
    '48x48.png',
    '64x64.png',
    '72x72.png',
    '96x96.png',
    '128x128.png',
    '144x144.png',
    '160x160.png',
    '192x192.png',
    '256x256.png',
    '512x512.png',
    'scalable.svg',
]

foreach logo : logos
    name = logo.split('.')[0]
    ext = logo.split('.')[1]
    install_data(
        meson.project_source_root() / 'assets' / 'icon' / logo,
        install_dir: datadir / 'icons' / 'hicolor' / name / 'apps',
        rename: [app_name + '.' + ext],
    )
endforeach
