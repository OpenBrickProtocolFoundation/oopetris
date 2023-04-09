#!/usr/bin/env bash

export BASE_PATH="/home/totto/Android/Sdk"

export PATH="$BASE_PATH/ndk/25.2.9519653:$PATH"                  # for 'ndk-build'
export PATH="$BASE_PATH/tools:$PATH"           # for 'android'
export PATH="$BASE_PATH/tools/bin:$PATH"           # for 'android'
export PATH="$BASE_PATH/platform-tools:$PATH"  # for 'adb'
export ANDROID_HOME="$BASE_PATH"        # for gradle
export ANDROID_NDK_HOME="$BASE_PATH/ndk/25.2.9519653"     # for gradle

export PATH="$ANDROID_NDK_HOME/bin:$PATH"

export PATH="/home/totto/Code/coder2k/oopetris/toolchain/android-ndk-r25c/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH"

OUTDIR="$PWD/build/out"
CC=armv7a-linux-androideabi33-clang CXX=armv7a-linux-androideabi33-clang++ CFLAGS="-L$OUTDIR/lib -I$OUTDIR/include -I/usr/include/x86_64-linux-gnu" LDFLAGS="-L$OUTDIR/lib" CXXFLAGS="-L$OUTDIR/lib" meson setup build --cross-file ./android/crossbuilt.txt --prefix=$OUTDIR

meson compile -C build