#!/usr/bin/env bash

set -xe

mkdir -p toolchains

# cd toolchains

# export NDK_VER="25c"

# wget "https://dl.google.com/android/repository/android-ndk-$NDK_VER-linux.zip"

# unzip -q "android-ndk-r$NDK_VER-linux.zip"

# cd ..

export BASE_PATH="$PWD/toolchains/android-ndk-r$NDK_VER"

export BIN_DIR="$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/bin"

export PATH="$BIN_DIR:$PATH"

export SDK_VERSION="33"

cat <<EOF >./android/crossbuilt.txt
[host_machine]
system = 'android'
cpu_family = 'arm'
cpu = 'arm'
endian = 'little'

[constants]
android_ndk = '$BIN_DIR'
toolchain = '$BIN_DIR/armv7a-linux-androideabi-$SDK_VERSION'
outdir = '$PWD/build/out'

[binaries]
c = 'armv7a-linux-androideabi$SDK_VERSION-clang'
cpp = 'armv7a-linux-androideabi$SDK_VERSION-clang++'
ar = 'llvm-ar'
ld = 'llvm-link'
objcopy = 'llvm-objcop'
strip = 'llvm-strip'
pkgconfig = ''

[built-in options]
c_std = 'c11'
cpp_std = 'c++20'
prefix = '$PWD/build/out'
c_args = ['-fPIC','-I$PWD/build/out/include']
cpp_args = ['-fPIC','-I$PWD/build/out/include']
c_link_args = ['-fPIE','-L$PWD/build/out/lib']
cpp_link_args = ['-fPIE','-L$PWD/build/out/lib']

[properties]
sys_root = '$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot'


EOF

export OUTDIR="$PWD/build/out"
CC=armv7a-linux-androideabi$SDK_VERSION-clang CXX=armv7a-linux-androideabi$SDK_VERSION-clang++ CFLAGS="-L$OUTDIR/lib -I$OUTDIR/include" LDFLAGS="-L$OUTDIR/lib" CXXFLAGS="-L$OUTDIR/lib" meson setup build --cross-file ./android/crossbuilt.txt --prefix=$OUTDIR

meson compile -C build
