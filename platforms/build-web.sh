#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status.
set -e
## Treat undefined variables as an error
set -u
# fails if any part of a pipeline (|) fails
set -o pipefail

## options: "smart, complete_rebuild"
export COMPILE_TYPE="smart"

export BUILDTYPE="debug"

export RUN_IN_CI="false"

if [ "$#" -eq 0 ]; then
    # nothing
    echo "Using compile type '$COMPILE_TYPE'"
elif [ "$#" -eq 1 ]; then
    COMPILE_TYPE="$1"
elif [ "$#" -eq 2 ]; then
    COMPILE_TYPE="$1"
    BUILDTYPE="$2"
elif [ "$#" -eq 3 ]; then
    COMPILE_TYPE="$1"
    BUILDTYPE="$2"

    if [ -z "$3" ]; then
        RUN_IN_CI="false"
    else
        RUN_IN_CI="true"
    fi
else
    echo "Too many arguments given, expected 1, 2 or 3" >&2
    exit 1
fi

if [ "$COMPILE_TYPE" == "smart" ]; then
    : # noop
elif [ "$COMPILE_TYPE" == "complete_rebuild" ]; then
    : # noop
else
    echo "Invalid COMPILE_TYPE, expected: 'smart' or 'complete_rebuild'" >&2
    exit 1
fi

if [ ! -d "toolchains" ]; then
    mkdir -p toolchains
fi

# source dependency version information

SCRIPT_DIR="$(realpath "$(dirname -- "${BASH_SOURCE[0]}")")"

# shellcheck source=./platforms/versions.sh
source "$SCRIPT_DIR/versions.sh"

# shellcheck source=./platforms/helper.sh
source "$SCRIPT_DIR/helper.sh"

EMSCRIPTEN_ROOT="$(pwd)/toolchains/emsdk"
export EMSCRIPTEN_ROOT

if [ ! -d "$EMSCRIPTEN_ROOT" ]; then
    git clone https://github.com/emscripten-core/emsdk.git "$EMSCRIPTEN_ROOT"
else
    git -C "$EMSCRIPTEN_ROOT" fetch
fi

git -C "$EMSCRIPTEN_ROOT" checkout "$EMSCRIPTEN_RELEASE_TAG"

export EMSDK_NODE_TOOL="node-$EMSDK_EXECUTE_NODE_VERSION-64bit"

"$EMSCRIPTEN_ROOT/emsdk" install "$EMSCRIPTEN_RELEASE_TAG" "$EMSDK_NODE_TOOL"
"$EMSCRIPTEN_ROOT/emsdk" activate "$EMSCRIPTEN_RELEASE_TAG" "$EMSDK_NODE_TOOL"

EMSCRIPTEN_UPSTREAM_ROOT="$EMSCRIPTEN_ROOT/upstream/emscripten"

EMSCRIPTEN_PATCH_FILE="$EMSCRIPTEN_UPSTREAM_ROOT/.patched_manually.meta"

PATCH_DIR="platforms/emscripten"

if ! [ -e "$EMSCRIPTEN_PATCH_FILE" ]; then
    ##TODO: upstream those patches
    # see: https://github.com/emscripten-core/emscripten/pull/18379
    # and: https://github.com/emscripten-core/emscripten/pull/22946

    git apply --unsafe-paths -p1 --directory="$EMSCRIPTEN_UPSTREAM_ROOT" "$PATCH_DIR/sdl2_image_port.diff"

    touch "$EMSCRIPTEN_PATCH_FILE"
fi

# git apply path

# shellcheck disable=SC1091
EMSDK_QUIET=1 source "$EMSCRIPTEN_ROOT/emsdk_env.sh" >/dev/null

## build the needed dependencies
embuilder build sdl2-mt harfbuzz-mt freetype zlib sdl2_ttf mpg123 "sdl2_mixer-mp3-mt" libpng-mt "sdl2_image:formats=png,svg:mt=1" icu-mt

export EMSCRIPTEN_SYS_ROOT="$EMSCRIPTEN_UPSTREAM_ROOT/cache/sysroot"

export BUILD_DIR="build/web"

export PKG_CONFIG_PATH="$EMSCRIPTEN_SYS_ROOT/lib/pkgconfig"

export CC="emcc"
export CXX="em++"
export AR="emar"
export RANLIB="emranlib"
export STRIP="emstrip"
export NM="emnm"

export ARCH="wasm32"
export CPU_ARCH="wasm32"
export ENDIANESS="little"

export ROMFS="platforms/romfs"

export PACKAGE_FLAGS="'--use-port=sdl2', '--use-port=harfbuzz', '--use-port=freetype', '--use-port=zlib', '--use-port=sdl2_ttf', '--use-port=mpg123', '--use-port=sdl2_mixer', '-sSDL2_MIXER_FORMATS=[\"mp3\"]','--use-port=libpng', '--use-port=sdl2_image','-sSDL2_IMAGE_FORMATS=[\"png\",\"svg\"]', '--use-port=icu'"

#TODO use '-sMEMORY64', '-m64',  and target wasm64
export COMMON_FLAGS="'-fexceptions', '-pthread', '-sUSE_PTHREADS=1', '-sEXCEPTION_CATCHING_ALLOWED=[..]', $PACKAGE_FLAGS"

# TODO see if ALLOW_MEMORY_GROWTH is needed, but if we load ttf's and music it likely is and we don't have to debug OOm crashes, that aren't handled by some third party library, which is painful
export LINK_FLAGS="$COMMON_FLAGS, '-sEXPORT_ALL=1', '-sWASM=1', '-sALLOW_MEMORY_GROWTH=1', '-sASSERTIONS=1','-sERROR_ON_UNDEFINED_SYMBOLS=1', '-sFETCH=1', '-sEXIT_RUNTIME=1'"
export COMPILE_FLAGS="$COMMON_FLAGS ,'-DAUDIO_PREFER_MP3'"

export CROSS_FILE="./platforms/crossbuild/web.ini"

validate_parent_dir "$CROSS_FILE"

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
emscripten_root = '$EMSCRIPTEN_ROOT'

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
pkg_config_libdir = '$PKG_CONFIG_PATH'
needs_exe_wrapper = true
sys_root = '$EMSCRIPTEN_SYS_ROOT'

APP_ROMFS='$ROMFS/assets/'

[cmake]

CMAKE_FIND_ROOT_PATH_MODE_PROGRAM  = 'BOTH'
CMAKE_FIND_ROOT_PATH_MODE_LIBRARY  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_INCLUDE  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_PACKAGE  = 'ONLY'

CMAKE_FIND_ROOT_PATH = '$EMSCRIPTEN_SYS_ROOT/lib/cmake'

EOF

if [ ! -d "$ROMFS" ]; then

    mkdir -p "$ROMFS"

    cp -r assets "$ROMFS/"

fi

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

    meson setup "$BUILD_DIR" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=static \
        -Dtests=false \
        "-Drun_in_ci=$RUN_IN_CI" \
        --fatal-meson-warnings

fi

meson compile -C "$BUILD_DIR"

if [ -n "${ENABLE_TESTING:-}" ]; then

    meson test -C "$BUILD_DIR"

fi
