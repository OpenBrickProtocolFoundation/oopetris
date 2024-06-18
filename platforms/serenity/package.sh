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
    "-Dbuildtype=release"
)
depends=(
    'SDL2'
    'SDL2_image'
    'SDL2_mixer'
    'SDL2_ttf'
    'nlohmann-json'
)

configure() {
    run meson setup _build "${configopts[@]}"
}

build() {
    run meson compile -C _build
}

install() {
    export DESTDIR="${SERENITY_INSTALL_ROOT}"
    run meson install -C _build
}
