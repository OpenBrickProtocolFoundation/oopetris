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

#TODO generate switch.mk or use meson in the future

mkdir -p platforms/romfs

cp -r assets platforms/romfs/


make -f ./platforms/switch.mk -j $(nproc)
