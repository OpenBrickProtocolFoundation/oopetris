#!/usr/bin/env bash

set -e

export DEVKITPRO=/opt/devkitpro
export COMPILER_BIN="$DEVKITPRO/devkitA64/bin"
export PATH="$DEVKITPRO/tools/bin:$COMPILER_BIN:$PATH"

export PORTLIBS_PATH="$DEVKITPRO/portlibs"
export LIBNX=$DEVKITPRO/libnx

export PORTLIBS_LIB=$PORTLIBS_PATH/switch/lib
export LIBNX_LIB=$LIBNX/lib

export PKG_CONFIG_PATH="$PORTLIBS_LIB/pkgconfig/"

export ROMFS=platforms/romfs

export BUILD_DIR="build-switch"

export TOOL_PREFIX=aarch64-none-elf

export BIN_DIR=/opt/devkitpro/portlibs/switch/bin
export PKG_CONFIG_EXEC=$BIN_DIR/$TOOL_PREFIX-pkg-config
export CMAKE="$BIN_DIR/$TOOL_PREFIX-cmake"

export PATH="$BIN_DIR:$PATH"

export CC="$COMPILER_BIN/$TOOL_PREFIX-gcc"
export CXX="$COMPILER_BIN/$TOOL_PREFIX-g"++
# export LD="$COMPILER_BIN/$TOOL_PREFIX-ld"
export AS="$COMPILER_BIN/$TOOL_PREFIX-as"
export AR="$COMPILER_BIN/$TOOL_PREFIX-gcc-ar"
export RANLIB="$COMPILER_BIN/$TOOL_PREFIX-gcc-ranlib"
export NM="$COMPILER_BIN/$TOOL_PREFIX-gcc-nm"
export OBJCOPY="$COMPILER_BIN/$TOOL_PREFIX-objcopy"
export STRIP="$COMPILER_BIN/$TOOL_PREFIX-strip"

export ARCH=aarch64
export ARM_VERSION=armv8-a
export COMMON_FLAGS="'-D__SWITCH__','-D__LIBNX__','-DNOSTYLUS','-D_XOPEN_SOURCE'"

export ARCH_ARGS="'-march=armv8-a','-mtp=soft','-fPIE'"

export LD_LIBS_FLAGS="'-L$PORTLIBS_LIB','-L$LIBNX_LIB'"
export ASFLAGS="-g $ARCH_ARGS"

cat <<EOF >"./platforms/crossbuilt-switch.ini"
[host_machine]
system = 'switch'
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
pkgconfig = '$PKG_CONFIG_EXEC'
cmake='$CMAKE'
freetype-config='$BIN_DIR/freetype-config'
libpng16-config='$BIN_DIR/libpng16-config'
libpng-config='$BIN_DIR/libpng-config'
sdl2-config='$BIN_DIR/sdl2-config'

[built-in options]
c_std = 'c11'
cpp_std = 'c++20'
c_args = [$ARCH_ARGS,$COMMON_FLAGS]
cpp_args = [$ARCH_ARGS,$COMMON_FLAGS]
c_link_args = ['-specs=$DEVKITPRO/libnx/switch.specs','-g', $ARCH_ARGS, $LD_LIBS_FLAGS]
cpp_link_args = ['-specs=$DEVKITPRO/libnx/switch.specs','-g', $ARCH_ARGS, $LD_LIBS_FLAGS]


[properties]
pkg_config_libdir = '$PKG_CONFIG_PATH'
needs_exe_wrapper = true
libray_dirs= ['$LIBNX_LIB', '$PORTLIBS_LIB']
romfs_dir='$ROMFS'
libnx='$LIBNX'

USE_NACP    = true
APP_TITLE	= 'oopetris'
APP_AUTHOR 	= 'coder2k'
APP_VERSION = '1.0'


EOF

mkdir -p $ROMFS

cp -r assets $ROMFS

meson setup "$BUILD_DIR" \
    --cross-file "./platforms/crossbuilt-switch.ini"

meson compile -C "$BUILD_DIR"
