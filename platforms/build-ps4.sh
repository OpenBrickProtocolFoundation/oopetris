#!/usr/bin/env bash

set -e

if [ -z "$OO_PS4_TOOLCHAIN" ]; then
    echo "Error, you need to set the 'OO_PS4_TOOLCHAIN' env variable"
    exit 1
fi

export TOOLCHAIN_ROOT="$OO_PS4_TOOLCHAIN"

export TOOLS_BIN="$TOOLCHAIN_ROOT/bin/linux"
export PATH="$TOOLS_BIN:$PATH"

export BUILD_DIR="build-ps4"

export ARCH="x86_64"
export CPU_ARCH="TODO"
export ENDIANESS="TODO"

# extra compile flags
export EXTRA_COMPILE_FLAGS=""

export COMPILE_FLAGS="'-D__PS4__','-D__CONSOLE__','--target=x86_64-pc-freebsd12-elf','-fPIC','-funwind-tables','-isysroot','$TOOLCHAIN_ROOT','-isystem','$TOOLCHAIN_ROOT/include'"

export CXX_FLAGS="$COMPILE_FLAGS ,'-isystem','$TOOLCHAIN_ROOT/include/c++/v1'"

export LINK_FLAGS="'-L$TOOLCHAIN_ROOT/lib'"

export LDFLAGS="-m elf_x86_64 -pie --script $TOOLCHAIN_ROOT/link.x --eh-frame-hdr $(TOOLCHAIN)/lib/crt1.o"

export CROSS_FILE="./platforms/crossbuild-ps4.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = 'ps4'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[target_machine]
system = 'ps4'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[constants]
ps4_toolchain = '$OO_PS4_TOOLCHAIN'


[binaries]
c_ld = 'ld'

[built-in options]
c_std = 'gnu11'
cpp_std = 'c++23'
c_args = [$COMPILE_FLAGS, $EXTRA_COMPILE_FLAGS]
cpp_args = [$CXX_FLAGS, $EXTRA_COMPILE_FLAGS]
c_link_args = [$LINK_FLAGS]
cpp_link_args = [$LINK_FLAGS]


[properties]
needs_exe_wrapper = true

TITLE_NAME = 'oopetris'
TITLE_ID = 'OOPE00042'
CONTENT_ID = 'EP0000-OOPE00042_00-OOPETRIS00000000'
TITLE_VERSION = true

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

if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

    meson setup "$BUILD_DIR" \
        "--wipe" \
        --cross-file "$CROSS_FILE" \
        "-Dbuildtype=$BUILDTYPE" \
        -Ddefault_library=static

fi

meson compile -C "$BUILD_DIR"
