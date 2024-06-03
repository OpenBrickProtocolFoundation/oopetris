#!/usr/bin/env -S bash ../.port_include.sh
port='oopetris'
version='0.5.6'
files=(
    "git+https://github.com/OpenBrickProtocolFoundation/oopetris#${version}"
)
useconfigure='true'
configopts=(
    '--cross-file'
    "${SERENITY_BUILD_DIR}/meson-cross-file.txt"
)
depends=(
    'SDL2'
    'SDL2_image'
    'SDL2_mixer'
    'SDL2_ttf'
    'nlohmann-json'
)

configure() {
    # TODO: Figure out why GCC doesn't autodetect that libgcc_s is needed. (stolen from glib port)
    if [ "${SERENITY_TOOLCHAIN}" = 'GNU' ]; then
        export LDFLAGS='-lgcc_s'
    fi

    run meson setup _build "${configopts[@]}"
}

build() {
    run meson compile -C _build
}

install() {
    export DESTDIR="${SERENITY_INSTALL_ROOT}"
    run meson install -C _build
}
