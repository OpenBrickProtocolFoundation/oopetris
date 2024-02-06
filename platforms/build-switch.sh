#!/usr/bin/env bash

set -e

export DEVKITPRO=/opt/devkitpro
export ARCH_DEVKIT_FOLDER="$DEVKITPRO/devkitA64"
export COMPILER_BIN="$ARCH_DEVKIT_FOLDER/bin"
export PATH="$DEVKITPRO/tools/bin:$COMPILER_BIN:$PATH"

export PORTLIBS_PATH="$DEVKITPRO/portlibs"
export LIBNX=$DEVKITPRO/libnx

export PORTLIBS_LIB=$PORTLIBS_PATH/switch/lib
export LIBNX_LIB=$LIBNX/lib

export PKG_CONFIG_PATH="$PORTLIBS_LIB/pkgconfig/"

export ROMFS="platforms/romfs"

export BUILD_DIR="build-switch"

export TOOL_PREFIX=aarch64-none-elf

export BIN_DIR="$PORTLIBS_PATH/switch/bin"
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
# compat flags for some POSIX functions
export COMPAT_FLAGS="'-D_XOPEN_SOURCE'"

export ARCH=aarch64
export CPU_ARCH=cortex-a57
export COMMON_FLAGS="'-D__SWITCH__','-ffunction-sections','-fdata-sections','-ftls-model=local-exec', '-march=armv8-a+crc+crypto','-mtune=cortex-a57','-mtp=soft','-ftls-model=local-exec','-fPIC', '-isystem', $COMPAT_FLAGS"

export COMPILE_FLAGS="'-isystem', '$LIBNX/include'"

export LINK_FLAGS="'-L$PORTLIBS_LIB','-L$LIBNX_LIB','-fPIE','-specs=$DEVKITPRO/libnx/switch.specs', '-lnx'"

export CROSS_FILE="./platforms/crossbuild-switch.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = 'switch'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
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
sdl2-config='$BIN_DIR/sdl2-config'

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
library_dirs= ['$LIBNX_LIB', '$PORTLIBS_LIB']
libnx='$LIBNX'

APP_ICON='$ROMFS/assets/icon/icon_512.png'
APP_TITLE	= 'oopetris'
APP_AUTHOR 	= 'coder2k'
APP_VERSION = '1.0'
APP_ROMFS='$ROMFS'

# optional
APP_TITLEID = ''

USE_NACP    = true

EOF

mkdir -p $ROMFS

cp -r assets $ROMFS

meson setup "$BUILD_DIR" \
    --cross-file "$CROSS_FILE" \
    -Ddefault_library=static

meson compile -C "$BUILD_DIR"
