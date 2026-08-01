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
    RUNTIME_TARGET="$4"
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

if [ "$RUNTIME_TARGET" == "emulator" ]; then
    : # noop
elif [ "$RUNTIME_TARGET" == "hardware" ]; then
    : # noop
else
    echo "Invalid RUNTIME_TARGET, expected: 'emulator' or 'hardware'" >&2
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

EDK2_LIBC_ROOT="$(pwd)/toolchains/edk2-libc"
export EDK2_LIBC_ROOT

if [ ! -d "$EDK2_LIBC_ROOT" ]; then
    git clone https://github.com/tianocore/edk2-libc "$EDK2_LIBC_ROOT"
else
    git -C "$EDK2_LIBC_ROOT" fetch
fi

git -C "$EDK2_LIBC_ROOT" checkout "$EDK2_LIBC_COMMIT_HASH"

export EDK_TOOLS_PATH="$EDK2_ROOT/BaseTools"
export PACKAGES_PATH="$EDK2_ROOT"

pushd "$EDK2_ROOT"

make -C BaseTools

set +u

# shellcheck disable=SC1091
source "$EDK2_ROOT/edksetup.sh" "--reconfig"

set -u

export EDK2_CONF_TARGET="$EDK2_ROOT/Conf/target.txt"

EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="EmulatorPkg/EmulatorPkg.dsc"

if [ "$RUNTIME_TARGET" == "emulator" ]; then
    EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="EmulatorPkg/EmulatorPkg.dsc"
elif [ "$RUNTIME_TARGET" == "hardware" ]; then
    EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="MdeModulePkg/MdeModulePkg.dsc"
else
    echo "Invalid RUNTIME_TARGET, expected: 'emulator' or 'hardware'" >&2
    exit 1
fi

EDK2_TARGET_PROPERTIES_BUILDTYPE="$(echo "$BUILDTYPE" | tr "[:lower:]" "[:upper:]")"

EDK2_TARGET_PROPERTIES_ARCH="X64"
EDK2_TARGET_PROPERTIES_TOOLCHAIN="GCC"

declare -A EDK2_TARGET_PROPERTIES=(["ACTIVE_PLATFORM"]="$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" ["TARGET"]="$EDK2_TARGET_PROPERTIES_BUILDTYPE" ["TARGET_ARCH"]="$EDK2_TARGET_PROPERTIES_ARCH" ["TOOL_CHAIN_TAG"]="$EDK2_TARGET_PROPERTIES_TOOLCHAIN")

for EDK2_TARGET_PROPERTY in "${!EDK2_TARGET_PROPERTIES[@]}"; do
    EDK2_TARGET_PROPERTY_VALUE="${EDK2_TARGET_PROPERTIES[$EDK2_TARGET_PROPERTY]}"

    sed -i "s|^${EDK2_TARGET_PROPERTY}[[:space:]]*=.*\$|${EDK2_TARGET_PROPERTY} = $EDK2_TARGET_PROPERTY_VALUE|" "$EDK2_CONF_TARGET"
done

export EDK2_LINUX_BIN_PATH="$EDK_TOOLS_PATH/Bin/Linux-x86_64"
export EDK2_POSIX_BIN_PATH="$EDK_TOOLS_PATH/BinWrappers/PosixLike"

export EDK2_BUILD_COMMAND="$EDK2_POSIX_BIN_PATH/build"

# add links to needed packages

set -x

EDK2_LIB_PACKAGES=("StdLib" "StdLibPrivateInternalFiles")

for EDK2_LIB_PACKAGE in "${EDK2_LIB_PACKAGES[@]}"; do

    if ! [ -e "$WORKSPACE/$EDK2_LIB_PACKAGE" ]; then
        ln -s "$EDK2_LIBC_ROOT/$EDK2_LIB_PACKAGE" "$WORKSPACE/$EDK2_LIB_PACKAGE"
    fi

done

export BUILD_DIR="build/uefi"

export BUILD_DIR_ABS="$(pwd)/$BUILD_DIR"

export EDK2_GENERATED_PACKAGES="$BUILD_DIR_ABS/GeneratedPackages"

if ! [ -e "$WORKSPACE/GeneratedPackages" ]; then
    mkdir -p "$EDK2_GENERATED_PACKAGES"
    ln -s "$EDK2_GENERATED_PACKAGES" "$WORKSPACE/GeneratedPackages"
fi

popd

export PKG_CONFIG_PATH="$EDK2_ROOT/lib/pkgconfig"

export CC="$SCRIPT_DIR/uefi/gcc-wrapper.sh"
export CXX="$SCRIPT_DIR/uefi/g++-wrapper.sh"
export AR="$SCRIPT_DIR/uefi/ar-wrapper.sh"
export RANLIB=""
export STRIP=""
export NM=""

export PKG_CONFIG_EXEC="$SCRIPT_DIR/uefi/pkg-config-wrapper.sh"

export ARCH="x86_64"
export CPU_ARCH="x86_64"
export ENDIANESS="little"

export ROMFS="platforms/romfs"

export COMMON_FLAGS="'-fexceptions', '-pthread'"

export LINK_FLAGS="$COMMON_FLAGS"
export COMPILE_FLAGS="$COMMON_FLAGS ,'-DAUDIO_PREFER_MP3'"

export CROSS_FILE="./platforms/crossbuild/uefi.ini"

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


pkg-config = '$PKG_CONFIG_EXEC'
cmake = ''

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

APP_ROMFS='$ROMFS/assets/'

[cmake]

CMAKE_FIND_ROOT_PATH_MODE_PROGRAM  = 'BOTH'
CMAKE_FIND_ROOT_PATH_MODE_LIBRARY  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_INCLUDE  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_PACKAGE  = 'ONLY'

CMAKE_FIND_ROOT_PATH = ''

EOF

if [ ! -d "$ROMFS" ]; then

    mkdir -p "$ROMFS"

    cp -r assets "$ROMFS/"

fi

export EDK2_INFO_FILE="./platforms/crossbuild/uefi_info.json"

validate_parent_dir "$EDK2_INFO_FILE"

cat <<EOF >"$EDK2_INFO_FILE"
{
    "build": "$EDK2_BUILD_COMMAND",
    "arch": "$EDK2_TARGET_PROPERTIES_ARCH",
    "workspace": "$WORKSPACE",
    "platform": "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM",
    "buildtype": "$EDK2_TARGET_PROPERTIES_BUILDTYPE",
    "toolchain": "$EDK2_TARGET_PROPERTIES_TOOLCHAIN"
    "generated_packages": "$EDK2_GENERATED_PACKAGES"
}
EOF

# "$EDK2_BUILD_COMMAND" -a "$EDK2_TARGET_PROPERTIES_ARCH" \
#     -p "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM" \
#     -m "$WORKSPACE/CustomPkg/HelloWorld.inf" \
#     -b "$EDK2_TARGET_PROPERTIES_BUILDTYPE" \
#     -t "$EDK2_TARGET_PROPERTIES_TOOLCHAIN" \
#     -w \
#     -v

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

    meson setup "$BUILD_DIR" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=static \
        -Dtests=false \
        --force-fallback-for="fmt,nlohmann_json,magic_enum,utf8cpp,sdl2_ttf,freetype2,spdlog,argparse" \
        --wrap-mode=nofallback \
        "-Drun_in_ci=$RUN_IN_CI" #TODO: enable \
    #--fatal-meson-warnings

fi

meson compile -C "$BUILD_DIR"

if [ -n "${ENABLE_TESTING:-}" ]; then

    meson test -C "$BUILD_DIR"

fi
