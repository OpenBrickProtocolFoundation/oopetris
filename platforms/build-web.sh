#!/usr/bin/env bash

set -e

if [ ! -d "toolchains" ]; then
    mkdir -p toolchains
fi

export EMSCRIPTEN_ROOT="$(pwd)/toolchains/emsdk"

if [ ! -d "$EMSCRIPTEN_ROOT" ]; then
    git clone https://github.com/emscripten-core/emsdk.git "$EMSCRIPTEN_ROOT"
fi

"$EMSCRIPTEN_ROOT/emsdk" install latest
"$EMSCRIPTEN_ROOT/emsdk" activate latest

# shellcheck disable=SC1091
EMSDK_QUIET=1 source "$EMSCRIPTEN_ROOT/emsdk_env.sh" >/dev/null

## build theneeded dependencies
embuilder build sdl2 harfbuzz freetype zlib sdl2_ttf vorbis mpg123 ogg libmodplug sdl2_mixer libpng libjpeg "sdl2_image:formats=png,jpg,svg" icu

export EMSCRIPTEN_SYS_ROOT="$EMSCRIPTEN_ROOT/upstream/emscripten/cache/sysroot"

export BUILD_DIR="build-web"

export CC="emcc"
export CXX="em++"
export AR="emar"
export RANLIB="emranlib"
export STRIP="emstrip"
export NM="emnm"

export ARCH="wasm32"
export CPU_ARCH="wasm32"
export ENDIANESS="little"

export PACKAGE_FLAGS="'--use-port=sdl2', '--use-port=harfbuzz', '--use-port=freetype', '--use-port=zlib', '--use-port=sdl2_ttf', '--use-port=vorbis', '--use-port=mpg123', '--use-port=ogg', '--use-port=libmodplug', '--use-port=sdl2_mixer', '--use-port=libpng', '--use-port=libjpeg', '--use-port=sdl2_image', '--use-port=icu'"

export COMMON_FLAGS="'-fexceptions', '-sEXCEPTION_CATCHING_ALLOWED=[..]', '-sFETCH', $PACKAGE_FLAGS"

# TODO see if ALLOW_MEMORY_GROWTH is needed, but if we load ttf's and music it likely is and we don't have to debug OOm crahses, that aren't handled by some thrid party library, which is painful
export LINK_FLAGS="$COMMON_FLAGS, '-sEXPORT_ALL=1', '-sUSE_WEBGPU=1', '-sWASM=1', '-sALLOW_MEMORY_GROWTH=1', '-sNO_EXIT_RUNTIME=0', '-sASSERTIONS=1','-sERROR_ON_UNDEFINED_SYMBOLS=1'"
export COMPILE_FLAGS="$COMMON_FLAGS ,'-DAUDIO_PREFER_MP3'"

export CROSS_FILE="./platforms/crossbuild-web.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = 'emscripten'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[target_machine]
system = 'emscripten'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[constants]
emscripten_root = '$(pwd)/emsdk'

[binaries]
c = '$CC'
cpp = '$CXX'
ar      = '$AR'
ranlib  = '$RANLIB'
strip   = '$STRIP'
nm = '$NM'

pkg-config = ['emmake', 'pkg-config']
cmake = ['emmake', 'cmake']
sdl2-config = ['emconfigure', 'sdl2-config']

exe_wrapper = '$EMSDK_NODE'

[built-in options]
c_std = 'c11'
cpp_std = 'c++23'
c_args = [$COMPILE_FLAGS]
cpp_args = [$COMPILE_FLAGS]
c_link_args = [$LINK_FLAGS]
cpp_link_args = [$LINK_FLAGS]

[properties]
needs_exe_wrapper = true
sys_root = '$EMSCRIPTEN_SYS_ROOT'


EOF

## options: "smart, complete_rebuild"
export COMPILE_TYPE="smart"

export BUILDTYPE="debug"

if [ "$#" -eq 0 ]; then
    # nothing
    echo "Using compile type '$COMPILE_TYPE'"
elif [ "$#" -eq 1 ]; then
    COMPILE_TYPE="$1"
elif [ "$#" -eq 2 ]; then
    COMPILE_TYPE="$1"
    BUILDTYPE="$2"
else
    echo "Too many arguments given, expected 1 or 2"
    exit 1
fi

if [ "$COMPILE_TYPE" == "smart" ]; then
    : # noop
elif [ "$COMPILE_TYPE" == "complete_rebuild" ]; then
    : # noop
else
    echo "Invalid COMPILE_TYPE, expected: 'smart' or 'complete_rebuild'"
    exit 1
fi

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

    meson setup "$BUILD_DIR" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=static

fi

meson compile -C "$BUILD_DIR"
