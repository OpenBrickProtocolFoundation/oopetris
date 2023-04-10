#!/usr/bin/env bash

set -e

mkdir -p toolchains

export NDK_VER="25c"

export BASE_PATH="$PWD/toolchains/android-ndk-r$NDK_VER"

if [ ! -d "$BASE_PATH" ]; then

    cd toolchains

    wget "https://dl.google.com/android/repository/android-ndk-r$NDK_VER-linux.zip"

    unzip -q "android-ndk-r$NDK_VER-linux.zip"

    cd ..
fi

export ARCH="arm"
export ARM_VERSION="armv7a"
export ABI="armeabi-v7a"

export SDK_VERSION="33"

export ANDROID_SDK_HOME="/home/totto/Android/Sdk"
export ANDROID_NDK_HOME="${ANDROID_SDK_HOME}/ndk/25.2.9519653"
export ANDROID_NDK="$ANDROID_NDK_HOME"
export BASE_PATH="$ANDROID_NDK_HOME"

export TOOLCHAIN="$BASE_PATH/build/cmake/android.toolchain.cmake"
export HOST_ROOT="$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64"
# export SYS_ROOT="${HOST_ROOT}/sysroot"
export SYS_ROOT="${HOST_ROOT}/sysroot_sym"
export BIN_DIR="$HOST_ROOT/bin"
export PATH="$BIN_DIR:$PATH"

LIB_PATH="${SYS_ROOT}/usr/lib/${ARCH}-linux-androideabi:${SYS_ROOT}/usr/lib/${ARCH}-linux-androideabi/${SDK_VERSION}"
INC_PATH="${SYS_ROOT}/usr/include"

# ln -s "${SYS_ROOT}/usr/lib/${ARCH}-linux-androideabi/${SDK_VERSION}" "$SYS_ROOT/usr"
# mv "$SYS_ROOT/usr/${SDK_VERSION}" "$SYS_ROOT/usr/lib_sym"

# XXX I not sure how much of this cmake config is actually having an impact
export CMAKE_PREFIX_PATH="$SYS_ROOT"
export CMAKE_ROOT="$ANDROID_SDK_HOME/cmake/3.22.1"
export CMAKE_LIBRARY_PATH="$LIB_PATH"
export CMAKE_INCLUDE_PATH="$INC_PATH"

export LIBRARY_PATH="$SYS_ROOT/usr/lib/$ARCH-linux-androideabi/$SDK_VERSION"

export CFLAGS="-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN} \
  -DANDROID_STL=c++_shared \
  -DANDROID_TOOLCHAIN=clang \
  -DANDROID_PLATFORM=android-${SDK_VERSION} \
  -DANDROID_ABI=${ABI} \
  -isystems '$LIBRARY_PATH' \
  -isystem'usr/lib/$ARCH-linux-androideabi/$SDK_VERSION'
  "
export CXXFLAGS="$CFLAGS"

LAST_DIR=$PWD

if [ ! -d "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/" ]; then

    mkdir -p "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/lib"

    cd "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/"

    ln -s "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/local" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/"

    ln -s "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/"

    find "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ARCH}-linux-androideabi/" -maxdepth 1 -name "*.so" -exec ln -s "{}" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/lib/" \;

    find "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ARCH}-linux-androideabi/" -maxdepth 1 -name "*.a" -exec ln -s "{}" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/lib/" \;

    find "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ARCH}-linux-androideabi/$SDK_VERSION/" -maxdepth 1 -name "*.a" -exec ln -s "{}" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/lib/" \;

    find "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ARCH}-linux-androideabi/$SDK_VERSION/" -maxdepth 1 -name "*.so" -exec ln -s "{}" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/lib/" \;

    find "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ARCH}-linux-androideabi/$SDK_VERSION/" -maxdepth 1 -name "*.o" -exec ln -s "{}" "$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym/usr/lib/" \;

    cd "$LAST_DIR"

fi

export CC=armv7a-linux-androideabi$SDK_VERSION-clang
export CPP=armv7a-linux-androideabi$SDK_VERSION-clang++
export CXX=$CPP
export LD=llvm-ld
export AS=llvm-as
export AR=llvm-ar
export RANLIB=llvm-ranlib

cat <<EOF >./android/crossbuilt.ini
[host_machine]
system = 'android'
cpu_family = '$ARCH'
cpu = '$ARM_VERSION'
endian = 'little'

[constants]
android_ndk = '$BIN_DIR'
toolchain = '$BIN_DIR/armv7a-linux-androideabi-$SDK_VERSION'

[binaries]
c = 'armv7a-linux-androideabi$SDK_VERSION-clang'
cpp = 'armv7a-linux-androideabi$SDK_VERSION-clang++'
ar      = 'llvm-ar'
as      = 'llvm-as'
ranlib  = 'llvm-ranlib'
ld      = 'llvm-link'
strip   = 'llvm-strip'
objcopy = 'llvm-objcop'
pkgconfig = 'false'
llvm-config = 'llvm-config'

[built-in options]
c_std = 'c11'
cpp_std = 'c++20'
c_args = ['--sysroot=$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym','-fPIE','-fPIC','--target=$ARM_VERSION-none-linux-androideabi','-DHAVE_USR_INCLUDE_MALLOC_H','-D_MALLOC_H', '-I/home/totto/Android/Sdk/ndk/25.2.9519653/sources/android/cpufeatures/']
cpp_args = ['--sysroot=$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym','-fPIE','-fPIC','--target=$ARM_VERSION-none-linux-androideabi']
c_link_args = ['-fPIE']
cpp_link_args = ['-fPIE']
prefix = '$SYS_ROOT'
libdir = '$LIB_PATH'

[properties]
pkg_config_libdir = '$LIB_PATH'
sys_root = '$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64/sysroot_sym'

EOF

export BUILD_DIR="build"

export LIBRARY_PATH="$LIBRARY_PATH:usr/lib/$ARCH-linux-androideabi/$SDK_VERSION:$LIB_PATH"

meson setup $BUILD_DIR \
    --prefix=${SYS_ROOT} \
    --includedir=${INC_PATH} \
    "--libdir=usr/lib/$ARCH-linux-androideabi/$SDK_VERSION" \
    "--build.cmake-prefix-path=${SYS_ROOT}" \
    --cross-file ./android/crossbuilt.ini \
    -Dsdl2:use_hidapi=disabled \
    -Dsdl2:NDK_ROOT=$ANDROID_NDK \
    -Dsdl2:test=false

meson compile -C $BUILD_DIR
