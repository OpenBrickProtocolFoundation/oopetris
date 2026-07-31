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

export RUNTIME_TARGET="emulator"

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
elif [ "$#" -eq 4 ]; then
    COMPILE_TYPE="$1"
    BUILDTYPE="$2"

    if [ -z "$3" ]; then
        RUN_IN_CI="false"
    else
        RUN_IN_CI="true"
    fi
    RUNTIME_TARGET="$2"
else
    echo "Too many arguments given, expected 1, 2 or 3"
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

if [ "$RUNTIME_TARGET" == "emulator" ]; then
    : # noop
elif [ "$RUNTIME_TARGET" == "hardware" ]; then
    : # noop
else
    echo "Invalid RUNTIME_TARGET, expected: 'emulator' or 'hardware'"
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

EDK2_ROOT="$(pwd)/toolchains/edk2"
export EDK2_ROOT

export WORKSPACE="$EDK2_ROOT"

if [ ! -d "$EDK2_ROOT" ]; then
    git clone https://github.com/tianocore/edk2 "$EDK2_ROOT"
else
    git -C "$EDK2_ROOT" fetch
fi

git -C "$EDK2_ROOT" checkout "$EDK2_RELEASE_TAG"
git -C "$EDK2_ROOT" submodule update --init

export EDK_TOOLS_PATH="$EDK2_ROOT/BaseTools"
export PACKAGES_PATH="$EDK2_ROOT"


pushd "$EDK2_ROOT"

make -C BaseTools

set +u

# shellcheck disable=SC1091
source "$EDK2_ROOT/edksetup.sh"

set -u

export EDK2_CONF_TARGET="$EDK2_ROOT/Conf/target.txt"

EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="EmulatorPkg/EmulatorPkg.dsc"

if [ "$RUNTIME_TARGET" == "emulator" ]; then
    EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="EmulatorPkg/EmulatorPkg.dsc"
elif [ "$RUNTIME_TARGET" == "hardware" ]; then
    EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="MdeModulePkg/MdeModulePkg.dsc"
else
    echo "Invalid RUNTIME_TARGET, expected: 'emulator' or 'hardware'"
    exit 1
fi

declare -A EDK2_TARGET_PROPERTIES=(["ACTIVE_PLATFORM"]="$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" ["TARGET"]="$(echo "$BUILDTYPE" | tr "[:lower:]" "[:upper:]")" ["TARGET_ARCH"]="X64" ["TOOL_CHAIN_TAG"]="GCC")

for EDK2_TARGET_PROPERTY in "${!EDK2_TARGET_PROPERTIES[@]}"; do
    EDK2_TARGET_PROPERTY_VALUE="${EDK2_TARGET_PROPERTIES[$EDK2_TARGET_PROPERTY]}"

    sed -i "s|^${EDK2_TARGET_PROPERTY}[[:space:]]*=.*\$|${EDK2_TARGET_PROPERTY} = $EDK2_TARGET_PROPERTY_VALUE|" "$EDK2_CONF_TARGET"
done

export EDK2_LINUX_BIN_PATH="$EDK_TOOLS_PATH/Bin/Linux-x86_64"
export EDK2_POSIX_BIN_PATH="$EDK_TOOLS_PATH/BinWrappers/PosixLike"


export EDK2_BUILD_COMMAND="$EDK2_POSIX_BIN_PATH/build"

popd

exit 1

## build the needed dependencies
embuilder build sdl2-mt harfbuzz-mt freetype zlib sdl2_ttf mpg123 "sdl2_mixer-mp3-mt" libpng-mt "sdl2_image:formats=png,svg:mt=1" icu-mt

export EDK2_SYS_ROOT="$EDK2_UPSTREAM_ROOT/cache/sysroot"

export BUILD_DIR="build/web"

export PKG_CONFIG_PATH="$EDK2_SYS_ROOT/lib/pkgconfig"

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
system = 'uefi'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[target_machine]
system = 'uefi'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[constants]
edk2_root = '$EDK2_ROOT'

[binaries]
c = '$CC'
cpp = '$CXX'
ar      = '$AR'
ranlib  = '$RANLIB'
strip   = '$STRIP'
nm = '$NM'

# pkg-config = ['emmake', 'pkg-config']
# cmake = ['emmake', 'cmake']
# sdl2-config = ['emconfigure', 'sdl2-config']

# exe_wrapper = '$EMSDK_NODE'

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
sys_root = '$EDK2_SYS_ROOT'

APP_ROMFS='$ROMFS/assets/'

[cmake]

CMAKE_FIND_ROOT_PATH_MODE_PROGRAM  = 'BOTH'
CMAKE_FIND_ROOT_PATH_MODE_LIBRARY  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_INCLUDE  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_PACKAGE  = 'ONLY'

CMAKE_FIND_ROOT_PATH = '$EDK2_SYS_ROOT/lib/cmake'

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
