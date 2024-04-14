#!/usr/bin/env bash

set -e

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

export PKG_CONFIG_PATH_PORTLIB="$PORTLIBS_LIB/pkgconfig/"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH_PORTLIB"

export ROMFS="platforms/romfs"

export BUILD_DIR="build-3ds"

export TOOL_PREFIX="arm-none-eabi"

export BIN_DIR="$PORTLIBS_PATH_3DS/bin"
export PKG_CONFIG_EXEC="$BIN_DIR/$TOOL_PREFIX-pkg-config"
export CMAKE="$BIN_DIR/$TOOL_PREFIX-cmake"

export PATH="$BIN_DIR:$PATH"

export CC="$COMPILER_BIN/$TOOL_PREFIX-gcc"
export CXX="$COMPILER_BIN/$TOOL_PREFIX-g"++
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

export COMPILE_FLAGS="'-D_3DS','-D__3DS__','-D__CONSOLE__','-D__NINTENDO_CONSOLE__','-isystem','$LIBCTRU/include','-I$PORTLIBS_PATH_3DS/include'"

export LINK_FLAGS="'-L$PORTLIBS_LIB','-L$LIBCTRU_LIB','-fPIE','-specs=$ARCH_DEVKIT_FOLDER/$TOOL_PREFIX/lib/3dsx.specs'"

export CROSS_FILE="./platforms/crossbuild-3ds.ini"

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
c_args = [$COMMON_FLAGS, $COMPILE_FLAGS]
cpp_args = [$COMMON_FLAGS, $COMPILE_FLAGS]
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

EOF

## build sdl2 and libraries (ttf, mixer, image)

export SDL_TOP_BUILD_DIR="SDL2_local_build_3ds"
export SDL_BUILD_DIR="build_dir"

export SDL_ROOT_DIR="$PORTLIBS_PATH_3DS"

mkdir -p "$SDL_TOP_BUILD_DIR"

cd "$SDL_TOP_BUILD_DIR" || exit 1

echo "*" >.gitignore

mkdir -p "$SDL_ROOT_DIR"

# build sdl2

export SDL2_SRC_DIR="SDL2-2.28.5"

if [ ! -d "$SDL2_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz"
    tar xzf SDL2-2.28.5.tar.gz
    rm -rf SDL2-2.28.5.tar.gz

    cd $SDL2_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    cd ..

fi

export SDL_CMAKE_DIR="$SDL_ROOT_DIR/lib/cmake/SDL2"

# build sdl2_ttf

export SDL2_TTF_SRC_DIR="SDL2_ttf-2.22.0"

if [ ! -d "$SDL2_TTF_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-2.22.0.tar.gz"
    tar xzf SDL2_ttf-2.22.0.tar.gz
    rm -rf SDL2_ttf-2.22.0.tar.gz

    cd $SDL2_TTF_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DSDL2_DIR=$SDL_CMAKE_DIR" "-DSDL2TTF_SAMPLES=OFF" "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    cd ..

fi

# build sdl2_mixer

export SDL2_MIXER_SRC_DIR="SDL2_mixer-2.8.0"

if [ ! -d "$SDL2_MIXER_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-2.8.0.tar.gz"
    tar xzf SDL2_mixer-2.8.0.tar.gz
    rm -rf SDL2_mixer-2.8.0.tar.gz

    cd $SDL2_MIXER_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DSDL2_DIR=$SDL_CMAKE_DIR" "-DSDL2MIXER_DEPS_SHARED=OFF" "-DSDL2MIXER_MIDI=OFF" "-DSDL2MIXER_WAVPACK=OFF" "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    cd ..

fi

# build sdl2_image

export SDL2_IMAGE_SRC_DIR="SDL2_image-2.8.2"

if [ ! -d "$SDL2_IMAGE_SRC_DIR" ]; then

    wget "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2.tar.gz"
    tar xzf SDL2_image-2.8.2.tar.gz
    rm -rf SDL2_image-2.8.2.tar.gz

    cd $SDL2_IMAGE_SRC_DIR

    cmake -S. "-B$SDL_BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release "-DSDL2_DIR=$SDL_CMAKE_DIR" "-DCMAKE_INSTALL_PREFIX=$SDL_ROOT_DIR/"
    cmake --build "$SDL_BUILD_DIR"
    cmake --install "$SDL_BUILD_DIR" --prefix "$SDL_ROOT_DIR/"

    cd ..

fi

# exit the build tree

cd ..

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

if [ ! -d "$ROMFS" ]; then

    mkdir -p "$ROMFS"

    cp -r assets "$ROMFS"

fi

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

    meson setup "$BUILD_DIR" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=static

fi

meson compile -C "$BUILD_DIR"
