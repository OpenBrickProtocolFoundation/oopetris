#!/usr/bin/env bash

set -e

export DEVKITPRO="/opt/devkitpro"
export ARCH_DEVKIT_FOLDER="$DEVKITPRO/devkitA64"
export COMPILER_BIN="$ARCH_DEVKIT_FOLDER/bin"
export PATH="$DEVKITPRO/tools/bin:$COMPILER_BIN:$PATH"

export PORTLIBS_PATH="$DEVKITPRO/portlibs"
export PORTLIBS_PATH_SWITCH="$PORTLIBS_PATH/switch"
export LIBNX="$DEVKITPRO/libnx"

export PORTLIBS_LIB="$PORTLIBS_PATH_SWITCH/lib"
export LIBNX_LIB="$LIBNX/lib"

export PKG_CONFIG_PATH="$PORTLIBS_LIB/pkgconfig/"

export ROMFS="platforms/romfs"

export BUILD_DIR="build-switch"

export TOOL_PREFIX="aarch64-none-elf"

export BIN_DIR="$PORTLIBS_PATH_SWITCH/bin"
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

export ARCH="aarch64"
export CPU_ARCH="cortex-a57"
export ENDIANESS="little"

export COMMON_FLAGS="'-ftls-model=local-exec','-march=armv8-a+crc+crypto','-mtune=cortex-a57','-mtp=soft','-ftls-model=local-exec','-fPIC','-ffunction-sections','-fdata-sections'"

# compat flags for some POSIX functions
export EXTRA_COMPILE_FLAGS="'-D_XOPEN_SOURCE'"

export COMPILE_FLAGS="'-D__SWITCH__','-D__CONSOLE__','-D__NINTENDO_CONSOLE__','-isystem','$LIBNX/include','-I$PORTLIBS_PATH_SWITCH/include'"

export LINK_FLAGS="'-L$PORTLIBS_LIB','-L$LIBNX_LIB','-fPIE','-specs=$DEVKITPRO/libnx/switch.specs'"

export CROSS_FILE="./platforms/crossbuild-switch.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = 'switch'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[target_machine]
system = 'switch'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
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
sdl2-config='$BIN_DIR/sdl2-config'

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
library_dirs= ['$LIBNX_LIB','$PORTLIBS_LIB']
libnx='$LIBNX'

APP_NAME	= 'oopetris'
APP_AUTHOR 	= 'coder2k'
APP_VERSION = '0.5.4'

USE_NACP    = true

APP_ROMFS='$ROMFS'

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

if [ ! -d "$ROMFS" ]; then

    mkdir -p "$ROMFS"

    cp -r assets "$ROMFS/"

fi

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

    meson setup "$BUILD_DIR" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=static

fi

meson compile -C "$BUILD_DIR"
