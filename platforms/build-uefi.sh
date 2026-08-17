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

TOOLCHAIN_DIR="$(pwd)/toolchains"
export TOOLCHAIN_DIR

export EDK2_ROOT="$TOOLCHAIN_DIR/edk2"

export WORKSPACE="$EDK2_ROOT"

if [ ! -d "$EDK2_ROOT" ]; then
    git clone https://github.com/tianocore/edk2 "$EDK2_ROOT"
else
    git -C "$EDK2_ROOT" fetch
fi

git -C "$EDK2_ROOT" checkout "$EDK2_RELEASE_TAG"
git -C "$EDK2_ROOT" submodule update --init

export EDK2_LIBC_ROOT="$TOOLCHAIN_DIR/edk2-libc"

if [ ! -d "$EDK2_LIBC_ROOT" ]; then
    git clone https://github.com/tianocore/edk2-libc "$EDK2_LIBC_ROOT"
else
    git -C "$EDK2_LIBC_ROOT" fetch
fi

git -C "$EDK2_LIBC_ROOT" checkout "$EDK2_LIBC_COMMIT_HASH"

export EDK2_LLVM_ROOT="$TOOLCHAIN_DIR/llvm-project"

if [ ! -d "$EDK2_LLVM_ROOT" ]; then

    git clone --filter=blob:none --sparse https://github.com/Totto16/llvm-project.git "$EDK2_LLVM_ROOT"
    git -C "$EDK2_LLVM_ROOT" sparse-checkout set libcxx libc libcxxabi libunwind
else
    git -C "$EDK2_LLVM_ROOT" fetch
fi

git -C "$EDK2_LLVM_ROOT" checkout "$EDK2_LLVM_PORT_BRANCH"

export EDK_TOOLS_PATH="$EDK2_ROOT/BaseTools"
export PACKAGES_PATH="$EDK2_ROOT"

PLATFORMS_DIR="$(pwd)/platforms/uefi"

PATCH_DIR="$PLATFORMS_DIR"

EDK2_PATCH_FILE="$EDK2_ROOT/.patched_manually.meta"

if ! [ -e "$EDK2_PATCH_FILE" ]; then
    ##TODO: upstream those patches

    #TODO: use loop for this patches

    git apply --unsafe-paths -p1 --directory="$EDK2_ROOT" "$PATCH_DIR/cxx_compiler.diff"
    git apply --unsafe-paths -p1 --directory="$EDK2_ROOT" "$PATCH_DIR/ssl_lib.diff"
    git apply --unsafe-paths -p1 --directory="$EDK2_ROOT" "$PATCH_DIR/ssl_lib_compile_with_libc.diff"
    git apply --unsafe-paths -p1 --directory="$EDK2_ROOT" "$PATCH_DIR/use_arch_x86_64_v1.diff"

    touch "$EDK2_PATCH_FILE"
fi

EDK2_LIBC_PATCH_FILE="$EDK2_LIBC_ROOT/.patched_manually.meta"

if ! [ -e "$EDK2_LIBC_PATCH_FILE" ]; then
    ##TODO: upstream those patches

    git apply --unsafe-paths -p1 --directory="$EDK2_LIBC_ROOT" "$PATCH_DIR/libc_cpp_compatibility.diff"

    touch "$EDK2_LIBC_PATCH_FILE"
fi

export BUILD_DIR="build/uefi"

pushd "$EDK2_ROOT"

make -C BaseTools --quiet >/dev/null 2>&1

set +u

# shellcheck disable=SC1091
source "$EDK2_ROOT/edksetup.sh" "--reconfig"

set -u

export EDK2_CONF_TARGET="$EDK2_ROOT/Conf/target.txt"

EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME="Platforms/OOPetrisPlatform.dsc"

EDK2_TARGET_PROPERTIES_BUILDTYPE="$(echo "$BUILDTYPE" | tr "[:lower:]" "[:upper:]")"

EDK2_TARGET_PROPERTIES_ARCH="X64"
EDK2_TARGET_PROPERTIES_TOOLCHAIN="GCC"

declare -A EDK2_TARGET_PROPERTIES=(["ACTIVE_PLATFORM"]="$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME" ["TARGET"]="$EDK2_TARGET_PROPERTIES_BUILDTYPE" ["TARGET_ARCH"]="$EDK2_TARGET_PROPERTIES_ARCH" ["TOOL_CHAIN_TAG"]="$EDK2_TARGET_PROPERTIES_TOOLCHAIN")

for EDK2_TARGET_PROPERTY in "${!EDK2_TARGET_PROPERTIES[@]}"; do
    EDK2_TARGET_PROPERTY_VALUE="${EDK2_TARGET_PROPERTIES[$EDK2_TARGET_PROPERTY]}"

    sed -i "s|^${EDK2_TARGET_PROPERTY}[[:space:]]*=.*\$|${EDK2_TARGET_PROPERTY} = $EDK2_TARGET_PROPERTY_VALUE|" "$EDK2_CONF_TARGET"
done

export EDK2_LINUX_BIN_PATH="$EDK_TOOLS_PATH/Bin/Linux-x86_64"
export EDK2_POSIX_BIN_PATH="$EDK_TOOLS_PATH/BinWrappers/PosixLike"

export EDK2_BUILD_COMMAND="$EDK2_POSIX_BIN_PATH/build"

# add links to needed packages

EDK2_LIB_PACKAGES=("AppPkg" "StdLib" "StdLibPrivateInternalFiles")

for EDK2_LIB_PACKAGE in "${EDK2_LIB_PACKAGES[@]}"; do

    if ! [ -e "$WORKSPACE/$EDK2_LIB_PACKAGE" ]; then
        link_files_checked "$EDK2_LIBC_ROOT/$EDK2_LIB_PACKAGE" "$WORKSPACE/$EDK2_LIB_PACKAGE"
    fi

done

if ! [ -e "$WORKSPACE/LLVM" ]; then
    link_files_checked "$EDK2_LLVM_ROOT" "$WORKSPACE/LLVM"
fi

LIBRARY_PKG_ROOT="$WORKSPACE/LibraryPkg"

mkdir -p "$LIBRARY_PKG_ROOT"

EDK2_PORT_NAMES=("SDL2Pkg" "SDL2MixerPkg")
EDK2_PORT_URLS=("https://github.com/Totto16/SDL2_UEFI" "https://github.com/Totto16/SDL2_mixer_UEFI")
EDK2_PORT_TAGS=("uefi_port" "uefi_port")

for EDK2_PORT_KEY in "${!EDK2_PORT_NAMES[@]}"; do

    EDK2_PORT_NAME="${EDK2_PORT_NAMES[$EDK2_PORT_KEY]}"
    EDK2_PORT_URL="${EDK2_PORT_URLS[$EDK2_PORT_KEY]}"
    EDK2_PORT_TAG="${EDK2_PORT_TAGS[$EDK2_PORT_KEY]}"

    EDK2_PORT_ROOT="$TOOLCHAIN_DIR/$EDK2_PORT_NAME"

    if [ ! -d "$EDK2_PORT_ROOT" ]; then
        git clone "$EDK2_PORT_URL" "$EDK2_PORT_ROOT"
    else
        git -C "$EDK2_PORT_ROOT" fetch
    fi

    git -C "$EDK2_PORT_ROOT" checkout "$EDK2_PORT_TAG"

    if ! [ -e "$LIBRARY_PKG_ROOT/$EDK2_PORT_NAME" ]; then
        link_files_checked "$EDK2_PORT_ROOT" "$LIBRARY_PKG_ROOT/$EDK2_PORT_NAME"
    fi

done

popd

export CC="$SCRIPT_DIR/uefi/gcc-wrapper.sh"
export CXX="$SCRIPT_DIR/uefi/g++-wrapper.sh"

export AR="$SCRIPT_DIR/uefi/ar-wrapper.sh"
export RANLIB=""
export STRIP="false"
export NM="nm"

export PKG_CONFIG_EXEC="$SCRIPT_DIR/uefi/pkg-config-wrapper.sh"

export ARCH="x86_64"
export CPU_ARCH="x86_64"
export ENDIANESS="little"

export ROMFS="platforms/romfs"

export COMMON_FLAGS="'-m64', '-march=x86-64', '-maccumulate-outgoing-args', '-mno-red-zone', '-mcmodel=small'"

export SYS_ROOT="$WORKSPACE"

export PKG_CONFIG_PATH="$SYS_ROOT/lib/pkgconfig"

export LINK_FLAGS="$COMMON_FLAGS"
export COMPILE_FLAGS="$COMMON_FLAGS ,'--sysroot=${SYS_ROOT}', '-D__UEFI__', '-DEFIAPI=__attribute__((ms_abi))', '-fexceptions', '-fshort-wchar', '-fno-builtin', '-fno-strict-aliasing', '-fno-common', '-fstack-protector', '-ffunction-sections', '-fdata-sections', '-fno-asynchronous-unwind-tables', '-fno-omit-frame-pointer', '-DAUDIO_PREFER_MP3'"

export CC_COMPILE_FLAGS="'-nostdinc', '-I$WORKSPACE/StdLib/Include', '-I$WORKSPACE/StdLib/Include/$EDK2_TARGET_PROPERTIES_ARCH',  '-I$WORKSPACE/MdePkg/Include', '-I$WORKSPACE/MdePkg/Include/$EDK2_TARGET_PROPERTIES_ARCH'"

export CXX_COMPILE_FLAGS="'-fno-exceptions', '-fno-threadsafe-statics', '-fno-use-cxa-atexit', '-fno-unwind-tables', '-fno-asynchronous-unwind-tables', '-nostdinc++', '-nodefaultlibs', '-I$WORKSPACE/LLVM/libcxx/include', $CC_COMPILE_FLAGS"

export CROSS_FILE="./platforms/crossbuild/uefi.ini"

validate_parent_dir "$CROSS_FILE"

## note: UEFI_USE_GDB can be set via environment variables, to use gdb

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
c_args = [$COMPILE_FLAGS, $CC_COMPILE_FLAGS]
cpp_args = [$COMPILE_FLAGS, $CXX_COMPILE_FLAGS]
c_link_args = [$LINK_FLAGS]
cpp_link_args = [$LINK_FLAGS]

prefix = '$SYS_ROOT'

[properties]
pkg_config_libdir = '$PKG_CONFIG_PATH'
sys_root = '${SYS_ROOT}'
needs_exe_wrapper = true

APP_ROMFS='$ROMFS/assets/'
RUNTIME_TARGET='$RUNTIME_TARGET'
USE_GDB='${UEFI_USE_GDB:-false}'

[cmake]

CMAKE_FIND_ROOT_PATH_MODE_PROGRAM  = 'BOTH'
CMAKE_FIND_ROOT_PATH_MODE_LIBRARY  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_INCLUDE  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_PACKAGE  = 'ONLY'

CMAKE_FIND_ROOT_PATH = '$SYS_ROOT/usr/lib/cmake'

EOF

if [ ! -d "$ROMFS" ]; then

    mkdir -p "$ROMFS"

    cp -r assets "$ROMFS/"

fi

export EDK2_INFO_FILE="./platforms/crossbuild/uefi_info.json"

validate_parent_dir "$EDK2_INFO_FILE"

EDK2_GENERATED_PACKAGES="$(pwd)/$BUILD_DIR/GeneratedPackages"
export EDK2_GENERATED_PACKAGES

export EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM="$WORKSPACE/$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME"

cat <<EOF >"$EDK2_INFO_FILE"
{
    "build": "$EDK2_BUILD_COMMAND",
    "arch": "$EDK2_TARGET_PROPERTIES_ARCH",
    "workspace": "$WORKSPACE",
    "platform": "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM",
    "platform_name": "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM_NAME",
    "buildtype": "$EDK2_TARGET_PROPERTIES_BUILDTYPE",
    "toolchain": "$EDK2_TARGET_PROPERTIES_TOOLCHAIN",
    "generated_packages": "$EDK2_GENERATED_PACKAGES",
    "runtime_target": "$RUNTIME_TARGET"
}
EOF

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "${EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM:?}" ]; then
    rm -f "${EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM:?}"

    # copy template for oopetris platform

    mkdir -p "$(dirname "${EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM:?}")"

    cp "$PLATFORMS_DIR/OOPetrisPlatform.template.dsc" "${EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM:?}"

    # add stdlib to active platform

    STDLIB_PACKAGE_INCLUDE="!include StdLib/StdLib.inc"

    if grep -q "$STDLIB_PACKAGE_INCLUDE" "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"; then
        : # found already, do nothing
    else
        echo -e "$STDLIB_PACKAGE_INCLUDE\n" >>"$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"
    fi

    # add llvm to active platform

    LLVM_PACKAGE_INCLUDE="!include LLVM/LLVMPkg.inc"

    if grep -q "$LLVM_PACKAGE_INCLUDE" "$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"; then
        : # found already, do nothing
    else
        echo -e "$LLVM_PACKAGE_INCLUDE\n" >>"$EDK2_TARGET_PROPERTIES_ACTIVE_PLATFORM"
    fi

fi

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then
    rm -rf "$WORKSPACE/Build/"

    meson setup "$BUILD_DIR" \
        "--prefix=$SYS_ROOT" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=shared \
        -Dtests=false \
        --force-fallback-for="fmt,nlohmann_json,magic_enum,utf8cpp,sdl2_ttf,freetype2,spdlog,argparse,sdl2_image" \
        --wrap-mode=nofallback \
        "-Drun_in_ci=$RUN_IN_CI" \
        -Dstrip=false \
        --fatal-meson-warnings

    if ! [ -L "$WORKSPACE/GeneratedPackages" ]; then

        echo "mkdir: $EDK2_GENERATED_PACKAGES"
        mkdir -p "$EDK2_GENERATED_PACKAGES"
        link_files_checked "$EDK2_GENERATED_PACKAGES" "$WORKSPACE/GeneratedPackages"

    fi

fi

meson compile -C "$BUILD_DIR"

if [ -n "${ENABLE_TESTING:-}" ]; then

    meson test -C "$BUILD_DIR"

fi
