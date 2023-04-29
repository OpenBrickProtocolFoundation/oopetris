#!/usr/bin/env bash

set -e

if [ ! -d "emsdk" ]; then

    git clone https://github.com/emscripten-core/emsdk.git
    ./emsdk install latest
    ./emsdk activate latest

fi
source "./emsdk/emsdk_env.sh"

export BUILD_DIR="build-emcc"

export CC=emcc
export CXX=em++
# export LD=llvm-ld
# export AS=llvm-as
export AR=emar
# export RANLIB=llvm-ranlib
# export STRIP=llvm-strip
unset PKG_CONFIG

cat <<EOF >"./android/crossbuilt-web.ini"
[host_machine]
system = 'emscripten'
cpu_family = 'wasm'
cpu = 'wasm'
endian = 'little'

[binaries]
c = '$CC'
cpp = '$CXX'
strip = 'emstrip'
ar      = 'emar'
ranlib  = 'emranlib'

pkgconfig = ['emcmake']
cmake = ['emcmake']
sdl2-config = ['emconfigure', 'em-config']

exe_wrapper = 'node'

[properties]
needs_exe_wrapper = true
sys_root = './emsdk/upstream/emscripten/cache/sysroot'

[built-in options]
c_std = 'c11'
cpp_std = 'c++20'


EOF

meson setup "$BUILD_DIR" \
    --cross-file "./android/crossbuilt-web.ini" \
    -Dcpp_args=-DAUDIO_PREFER_MP3 \
    -Ddefault_library=static \
    -Dfreetype2:zlib=disabled

meson compile -C "$BUILD_DIR"

## wasm-ld: warning: function signature mismatch: lseek
