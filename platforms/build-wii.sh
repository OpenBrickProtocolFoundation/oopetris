#!/usr/bin/env bash

set -e

export DEVKITPRO="/opt/devkitpro"
export ARCH_DEVKIT_FOLDER="$DEVKITPRO/devkitPPC"
export COMPILER_BIN="$ARCH_DEVKIT_FOLDER/bin"
export PATH="$DEVKITPRO/tools/bin:$COMPILER_BIN:$PATH"

export OGC_CONSOLE="wii"
export OGC_SUBDIR="wii"
export OGC_MACHINE="rvl"

export PORTLIBS_PATH="$DEVKITPRO/portlibs"
export LIBOGC="$DEVKITPRO/libogc"

export PORTLIBS_PATH_OGC="$PORTLIBS_PATH/$OGC_CONSOLE"
export PORTLIBS_PATH_PPC="$PORTLIBS_PATH/ppc"

export PORTLIBS_LIB_OGC="$PORTLIBS_PATH_OGC/lib"
export PORTLIBS_LIB_PPC="$PORTLIBS_PATH_PPC/lib"
export LIBOGC_LIB="$LIBOGC/lib/$OGC_SUBDIR"

export PKG_CONFIG_PATH_OGC="$PORTLIBS_LIB_OGC/pkgconfig/"
export PKG_CONFIG_PATH_PPC="$PORTLIBS_LIB_PPC/pkgconfig/"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH_OGC:$PKG_CONFIG_PATH_PPC"

export ROMFS="platforms/romfs"

export BUILD_DIR="build-wii"

export TOOL_PREFIX="powerpc-eabi"

export BIN_DIR_OGC="$PORTLIBS_PATH_OGC/bin"
export BIN_DIR_PPC="$PORTLIBS_LIB_PPC/bin"
export PKG_CONFIG_EXEC="$BIN_DIR_OGC/$TOOL_PREFIX-pkg-config"
export CMAKE="$BIN_DIR_OGC/$TOOL_PREFIX-cmake"

export PATH="$BIN_DIR_PPC:$BIN_DIR_OGC:$PATH"

export CC="$COMPILER_BIN/$TOOL_PREFIX-gcc"
export CXX="$COMPILER_BIN/$TOOL_PREFIX-g++"
export AS="$COMPILER_BIN/$TOOL_PREFIX-as"
export AR="$COMPILER_BIN/$TOOL_PREFIX-gcc-ar"
export RANLIB="$COMPILER_BIN/$TOOL_PREFIX-gcc-ranlib"
export NM="$COMPILER_BIN/$TOOL_PREFIX-gcc-nm"
export OBJCOPY="$COMPILER_BIN/$TOOL_PREFIX-objcopy"
export STRIP="$COMPILER_BIN/$TOOL_PREFIX-strip"

export ARCH="ppc"
export CPU_VERSION="ppc750"
export ENDIANESS="big"

export COMMON_FLAGS="'-m${OGC_MACHINE}','-mcpu=750','-meabi','-mhard-float','-ffunction-sections','-fdata-sections'"

export COMPILE_FLAGS="'-D__WII__','-D__CONSOLE__','-D__NINTENDO_CONSOLE__','-D_OGC_','-DGEKKO','-isystem', '$LIBOGC/include', '-I$PORTLIBS_PATH_PPC/include', '-I$PORTLIBS_PATH_OGC/include'"

export LINK_FLAGS="'-L$LIBOGC_LIB','-L$PORTLIBS_LIB_PPC','-L$PORTLIBS_LIB_OGC'"

export CROSS_FILE="./platforms/crossbuild-wii.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = 'wii'
cpu_family = '$ARCH'
cpu = '$CPU_VERSION'
endian = '$ENDIANESS'

[target_machine]
system = 'wii'
cpu_family = '$ARCH'
cpu = '$CPU_VERSION'
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
freetype-config='$BIN_DIR_PPC/freetype-config'
libpng16-config='$BIN_DIR_PPC/libpng16-config'
libpng-config='$BIN_DIR_PPC/libpng-config'
sdl2-config='$BIN_DIR_OGC/sdl2-config'

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
library_dirs= ['$LIBOGC_LIB', '$PORTLIBS_LIB_OGC','$PORTLIBS_LIB_PPC']

USE_META_XML    = true


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

meson compile -C "$BUILD_DIR" -j 3
