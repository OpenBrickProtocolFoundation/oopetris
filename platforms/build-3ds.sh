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

export DEVKITPRO="/opt/devkitpro"
export ARCH_DEVKIT_FOLDER="$DEVKITPRO/devkitARM"
export COMPILER_BIN="$ARCH_DEVKIT_FOLDER/bin"
export ARCH_DEVKIT_LIB="$ARCH_DEVKIT_FOLDER/lib"
export PATH="$DEVKITPRO/tools/bin:$COMPILER_BIN:$PATH"

export PORTLIBS_PATH="$DEVKITPRO/portlibs"
export PORTLIBS_PATH_3DS="$PORTLIBS_PATH/3ds"
export LIBCTRU="$DEVKITPRO/libctru"

export PORTLIBS_LIB="$PORTLIBS_PATH_3DS/lib"
export LIBCTRU_LIB="$LIBCTRU/lib"

export PKG_CONFIG_PATH_PORTLIB="$PORTLIBS_LIB/pkgconfig"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH_PORTLIB"

export ROMFS="platforms/romfs"

export BUILD_DIR="build/3ds"

export TOOL_PREFIX="arm-none-eabi"


export BIN_DIR="$PORTLIBS_PATH_3DS/bin"
export PKG_CONFIG_EXEC="$BIN_DIR/$TOOL_PREFIX-pkg-config"
export CMAKE="$BIN_DIR/$TOOL_PREFIX-cmake"

export PATH="$BIN_DIR:$PATH"

export CC="$COMPILER_BIN/$TOOL_PREFIX-gcc"
export CXX="$COMPILER_BIN/$TOOL_PREFIX-g++"
export AS="$COMPILER_BIN/$TOOL_PREFIX-as"
export AR="$COMPILER_BIN/$TOOL_PREFIX-gcc-ar"
export RANLIB="$COMPILER_BIN/$TOOL_PREFIX-gcc-ranlib"
export NM="$COMPILER_BIN/$TOOL_PREFIX-gcc-nm"
export OBJCOPY="$COMPILER_BIN/$TOOL_PREFIX-objcopy"
export STRIP="$COMPILER_BIN/$TOOL_PREFIX-strip"

export ARCH="arm"
export ARM_VERSION="arm11mpcore"
export ENDIANESS="little"

export COMMON_FLAGS="'-march=armv6k','-mtune=mpcore','-mfloat-abi=hard','-mtp=soft','-mword-relocations','-ffunction-sections','-fdata-sections'"

# compat flags for some POSIX functions
export EXTRA_COMPILE_FLAGS="'-D_XOPEN_SOURCE'"

export COMPILE_FLAGS="'-D_3DS','-D__3DS__','-D__CONSOLE__','-D__NINTENDO_CONSOLE__','-isystem','$LIBCTRU/include','-I$PORTLIBS_PATH_3DS/include'"

export LINK_FLAGS="'-L$PORTLIBS_LIB','-L$LIBCTRU_LIB','-fPIE','-specs=$ARCH_DEVKIT_FOLDER/$TOOL_PREFIX/lib/3dsx.specs', '-lctru'"

# source dependency version information

SCRIPT_DIR="$(realpath "$(dirname -- "${BASH_SOURCE[0]}")")"

# shellcheck source=./platforms/versions.sh
source "$SCRIPT_DIR/versions.sh"

# shellcheck source=./platforms/helper.sh
source "$SCRIPT_DIR/helper.sh"

export CROSS_FILE="./platforms/crossbuild/3ds.ini"

validate_parent_dir "$CROSS_FILE"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = '3ds'
cpu_family = '$ARCH'
cpu = '$ARM_VERSION'
endian = '$ENDIANESS'

[target_machine]
system = '3ds'
cpu_family = '$ARCH'
cpu = '$ARM_VERSION'
endian = '$ENDIANESS'

[constants]
devkitpro = '$DEVKITPRO'

[binaries]
c = '$CC'
cpp = '$CXX'
c_ld = 'bfd'
cpp_ld = 'bfd'
ar      = '$AR'
as      = '$AS'
ranlib  = '$RANLIB'
strip   = '$STRIP'
objcopy = '$OBJCOPY'
nm = '$NM'
pkg-config = '$PKG_CONFIG_EXEC'
cmake='$CMAKE'
freetype-config='$BIN_DIR/freetype-config'
libpng16-config='$BIN_DIR/libpng16-config'
libpng-config='$BIN_DIR/libpng-config'
sdl2-config='$BIN_DIR/bin/sdl2-config'

[built-in options]
c_std = 'gnu11'
cpp_std = 'c++23'
c_args = [$COMMON_FLAGS, $COMPILE_FLAGS, $EXTRA_COMPILE_FLAGS]
cpp_args = [$COMMON_FLAGS, $COMPILE_FLAGS, $EXTRA_COMPILE_FLAGS]
c_link_args = [$COMMON_FLAGS, $LINK_FLAGS]
cpp_link_args = [$COMMON_FLAGS, $LINK_FLAGS]


[properties]
pkg_config_libdir = '$PKG_CONFIG_PATH'
needs_exe_wrapper = true
library_dirs= ['$LIBCTRU_LIB','$PORTLIBS_LIB']
libctru='$LIBCTRU'

APP_NAME	= 'oopetris'
APP_AUTHOR 	= 'coder2k'
APP_DESC = 'A Tetris clone in OOP'

USE_SMDH    = true

APP_ROMFS='$ROMFS'

[cmake]

CMAKE_FIND_ROOT_PATH_MODE_PROGRAM  = 'BOTH'
CMAKE_FIND_ROOT_PATH_MODE_LIBRARY  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_INCLUDE  = 'ONLY'
CMAKE_FIND_ROOT_PATH_MODE_PACKAGE  = 'ONLY'
CMAKE_SYSTEM_INCLUDE_PATH = '/include'
CMAKE_SYSTEM_LIBRARY_PATH = '/lib'
CMAKE_SYSTEM_PROGRAM_PATH = '/bin'

CMAKE_FIND_ROOT_PATH = '$PORTLIBS_LIB/cmake'

EOF

## build sdl2 and libraries (ttf, mixer, image)

export SDL_TOP_BUILD_DIR=".private/SDL2_local_build_3ds"
export SDL_BUILD_DIR="build_dir"

export SDL_ROOT_DIR="$PORTLIBS_PATH_3DS"

mkdir -p "$SDL_TOP_BUILD_DIR"

pushd "$SDL_TOP_BUILD_DIR" || exit 1

echo "*" >.gitignore

mkdir -p "$SDL_ROOT_DIR"

# build sdl2

export SDL2_SRC_DIR="SDL2-${SDL_3DS_VERSION}"

if [ ! -d "$SDL2_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_3DS_VERSION}/SDL2-${SDL_3DS_VERSION}.tar.gz"
    tar xzf "SDL2-${SDL_3DS_VERSION}.tar.gz"
    rm -rf "SDL2-${SDL_3DS_VERSION}.tar.gz"

    pushd "$SDL2_SRC_DIR"

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    popd

fi

export SDL_CMAKE_DIR="$SDL_ROOT_DIR/lib/cmake/SDL2"

# build sdl2_ttf

export SDL2_TTF_SRC_DIR="SDL2_ttf-${SDL_TTF_3DS_VERSION}"

if [ ! -d "$SDL2_TTF_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-${SDL_TTF_3DS_VERSION}/SDL2_ttf-${SDL_TTF_3DS_VERSION}.tar.gz"
    tar xzf "SDL2_ttf-${SDL_TTF_3DS_VERSION}.tar.gz"
    rm -rf "SDL2_ttf-${SDL_TTF_3DS_VERSION}.tar.gz"

    pushd $SDL2_TTF_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DSDL2_DIR=$SDL_CMAKE_DIR" "-DSDL2TTF_SAMPLES=OFF" "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    popd

fi

# build sdl2_mixer

export SDL2_MIXER_SRC_DIR="SDL2_mixer-${SDL_MIXER_3DS_VERSION}"

if [ ! -d "$SDL2_MIXER_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL_mixer/releases/download/release-${SDL_MIXER_3DS_VERSION}/SDL2_mixer-${SDL_MIXER_3DS_VERSION}.tar.gz"
    tar xzf "SDL2_mixer-${SDL_MIXER_3DS_VERSION}.tar.gz"
    rm -rf "SDL2_mixer-${SDL_MIXER_3DS_VERSION}.tar.gz"

    pushd $SDL2_MIXER_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DSDL2_DIR=$SDL_CMAKE_DIR" "-DSDL2MIXER_DEPS_SHARED=OFF" "-DSDL2MIXER_MIDI=OFF" "-DSDL2MIXER_WAVPACK=OFF" "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    popd

fi

# build sdl2_image

export SDL2_IMAGE_SRC_DIR="SDL2_image-${SDL_IMAGE_3DS_VERSION}"

if [ ! -d "$SDL2_IMAGE_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL_image/releases/download/release-${SDL_IMAGE_3DS_VERSION}/SDL2_image-${SDL_IMAGE_3DS_VERSION}.tar.gz"
    tar xzf "SDL2_image-${SDL_IMAGE_3DS_VERSION}.tar.gz"
    rm -rf "SDL2_image-${SDL_IMAGE_3DS_VERSION}.tar.gz"

    pushd $SDL2_IMAGE_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DSDL2_DIR=$SDL_CMAKE_DIR" "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    popd

fi

# exit the build tree

popd

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
        -Dcurl:tests=disabled \
        -Dcurl:unittests=disabled \
        -Dcurl:bearer-auth=enabled \
        -Dcurl:brotli=enabled \
        -Dcurl:libz=enabled \
        "-Drun_in_ci=$RUN_IN_CI" \
        --fatal-meson-warnings

fi

meson compile -C "$BUILD_DIR"
