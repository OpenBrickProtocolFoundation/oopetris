#!/usr/bin/env bash

set -e

export DEVKITPRO=/opt/devkitpro
export ARCH_DEVKIT_FOLDER="$DEVKITPRO/devkitARM"
export COMPILER_BIN="$ARCH_DEVKIT_FOLDER/bin"
export ARCH_DEVKIT_LIB="$ARCH_DEVKIT_FOLDER/lib"
export PATH="$DEVKITPRO/tools/bin:$COMPILER_BIN:$PATH"

export PORTLIBS_PATH="$DEVKITPRO/portlibs"
export LIBCTRU=$DEVKITPRO/libctru

export PORTLIBS_LIB=$PORTLIBS_PATH/3ds/lib
export LIBCTRU_LIB=$LIBCTRU/lib

export PKG_CONFIG_PATH="$PORTLIBS_LIB/pkgconfig/"

export ROMFS=platforms/romfs

export BUILD_DIR="build-3ds"

export TOOL_PREFIX=arm-none-eabi

export BIN_DIR="$PORTLIBS_PATH/3ds/bin"
export PKG_CONFIG_EXEC=$BIN_DIR/$TOOL_PREFIX-pkg-config
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

# compat flags for some POSIX functions, for some ABI errors, and spdlog thread local errors
export COMPAT_FLAGS="'-D_XOPEN_SOURCE','-Wno-psabi','-DSPDLOG_NO_TLS'"
export ARCH=arm
export ARM_VERSION=arm11mpcore
export COMMON_FLAGS="'-D__3DS__','-mword-relocations', '-ffunction-sections', '-fdata-sections', '-march=armv6k','-mtune=mpcore','-mfloat-abi=hard', '-mtp=soft', $COMPAT_FLAGS"

export COMPILE_FLAGS="'-isystem', '$LIBCTRU/include'"

export ARCH_SPEC_LIB="$ARCH_DEVKIT_FOLDER/$TOOL_PREFIX/lib"

export LINK_FLAGS="'-L$PORTLIBS_LIB','-L$LIBCTRU_LIB','-fPIE','-specs=$ARCH_SPEC_LIB/3dsx.specs', '-lctru'"

export CROSS_FILE="./platforms/crossbuild-3ds.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = '3ds'
cpu_family = '$ARCH'
cpu = '$ARM_VERSION'
endian = 'little'

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
sdl-config='$BIN_DIR/sdl-config'

[built-in options]
c_std = 'c11'
cpp_std = 'c++20'
c_args = [$COMMON_FLAGS, $COMPILE_FLAGS]
cpp_args = [$COMMON_FLAGS, $COMPILE_FLAGS]
c_link_args = [$COMMON_FLAGS, $LINK_FLAGS]
cpp_link_args = [$COMMON_FLAGS, $LINK_FLAGS]


[properties]
pkg_config_libdir = '$PKG_CONFIG_PATH'
needs_exe_wrapper = true
library_dirs= ['$LIBCTRU_LIB', '$PORTLIBS_LIB']
libctru='$LIBCTRU'

APP_ICON='$ROMFS/assets/icon/icon_48.png'
APP_SMALL_ICON = '$ROMFS/assets/icon/icon_24.png'
APP_NAME	= 'oopetris'
APP_AUTHOR 	= 'coder2k'
APP_DESC = 'OOP Tetris'
APP_ROMFS='$ROMFS'

USE_SMDH    = true

EOF

mkdir -p $ROMFS

cp -r assets $ROMFS

meson setup "$BUILD_DIR" \
    --cross-file "$CROSS_FILE" \
    -Ddefault_library=static 

meson compile -C "$BUILD_DIR"

## docs list:
# sdl 1.2: https://www.libsdl.org/release/SDL-1.2.15/docs/html/index.html
# sdl ttf tutorial: https://gist.github.com/stavrossk/5004111
# sdl gfx old version docs: https://www.cs.cmu.edu/afs/cs/user/meogata/Scramble/W%20i%20i/SDL_gfx/Docs/
# sdl 1.2 tutorial: http://gamedevgeek.com/tutorials/getting-started-with-sdl/
# sdl source code for 1.2, that is used: https://github.com/nop90/SDL-3DS

# https://wiibrew.org/wiki/SDL_Wii/tutorial
# https://wiibrew.org/wiki/Sdl
# https://wiibrew.org/wiki/SDL_Wii/tutorial
# https://wiibrew.org/wiki/SDL_Wii/Inputs
# https://wiki.libsdl.org/SDL2/README/n3ds

##  $DEVKITPRO/meson-cross.sh 3ds 3ds_crossfile_2.txt 3ds_build_2 && meson compile -C 3ds_build_2


# SDL_ttf crahses in Find_GlyphByIndex
