#!/usr/bin/env bash

set -e

export DEVKITPRO=/opt/devkitpro

export DEV_DIR=$DEVKITPRO/devkitA64
export BIN_DIR=$DEV_DIR/bin

export BUILD_DIR="build-switch"

export TOOL_PREFIX=aarch64-none-elf

export CC=$TOOL_PREFIX-gcc
export CPP=$TOOL_PREFIX-g++
export CXX=$CPP
export LD=$TOOL_PREFIX-ld
export AS=$TOOL_PREFIX-as
export AR=$TOOL_PREFIX-ar
export RANLIB=$TOOL_PREFIX-gcc-ranlib

export PATH="$BIN_DIR:$PATH"

#TODO generate switch.mk

make -f ./platforms/switch.mk

# cat <<EOF >"./android/crossbuilt-switch.ini"
# [host_machine]
# system = 'linux'
# cpu_family = 'arm64'
# cpu = 'aarch64'
# endian = 'little'

# [binaries]
# c = '$CC'
# cpp = '$CXX'
# ar      = '$AR'
# as      = '$AS'
# ranlib  = '$RANLIB'
# ld      = '$LD'
# strip   = '$TOOL_PREFIX-strip'
# objcopy = '$TOOL_PREFIX-objcop'
# pkgconfig = '$DEVKITPRO/portlibs/switch/bin/$TOOL_PREFIX-pkg-config'
# cmake = '$TOOL_PREFIX-cmake'

# [built-in options]
# c_std = 'c11'
# cpp_std = 'c++20'
# c_args = ['-fPIE','-fPIC']
# cpp_args = ['-fPIE','-fPIC']
# c_link_args = ['-fPIE']
# cpp_link_args = ['-fPIE']

# EOF

# meson setup "$BUILD_DIR" \
#     --cross-file "./android/crossbuilt-switch.ini" \
#     -Dsdl2:use_hidapi=disabled \
#     -Dsdl2:test=false

# meson compile -C "$BUILD_DIR"
