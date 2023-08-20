#!/usr/bin/env bash

set -e

mkdir -p toolchains

export NDK_VER_DOWNLOAD="r26-rc1"
export NDK_VER_DESC="r26-beta2"

export BASE_PATH="$PWD/toolchains/android-ndk-$NDK_VER_DESC"
export ANDROID_NDK_HOME="$BASE_PATH"
export ANDROID_NDK="$BASE_PATH"

if [ ! -d "$BASE_PATH" ]; then

    cd toolchains

    wget -q "https://dl.google.com/android/repository/android-ndk-$NDK_VER_DOWNLOAD-linux.zip"

    unzip -q "android-ndk-$NDK_VER_DOWNLOAD-linux.zip"

    cd ..
fi

if [ ! -e "$BASE_PATH/meta/abis.json" ]; then

    echo "no abis.json file found, to determine supported abis"
    exit 2

fi

if [ ! -e "$BASE_PATH/meta/platforms.json" ]; then

    echo "no platforms.json file found, to determine supported platforms and SDKs"
    exit 2

fi

export SDK_VERSION=$(jq '.max' -M -r -c "$BASE_PATH/meta/platforms.json")

mapfile -t ARCH_KEYS < <(jq 'keys' -M -r -c "$BASE_PATH/meta/abis.json" | tr -d '[]"' | sed 's/,/\n/g')

for INDEX in "${!ARCH_KEYS[@]}"; do
    export KEY=${ARCH_KEYS[$INDEX]}

    RAW_JSON=$(jq '.[$KEY]' -M -r -c --arg KEY "$KEY" "$BASE_PATH/meta/abis.json")

    BITNESS=$(echo "$RAW_JSON" | jq -M -r -c '."bitness"') || true
    ARCH=$(echo "$RAW_JSON" | jq -M -r -c '."arch"')
    ARM_VERSION=$(echo "$RAW_JSON" | jq -M -r -c '."proc"' | tr -d "-")
    ARM_NAME_TRIPLE=$(echo "$RAW_JSON" | jq -M -r -c '."triple"')
    ARM_TARGET_ARCH=$KEY
    ARM_TRIPLE=$ARM_NAME_TRIPLE$SDK_VERSION
    ARM_COMPILER_TRIPLE=$(echo "$RAW_JSON" | jq -M -r -c '."llvm_triple"')
    ARM_TOOL_TRIPLE=$(echo "$ARM_NAME_TRIPLE$SDK_VERSION" | sed s/$ARCH/$ARM_VERSION/)

    export SYM_LINK_PATH=sysroot_sym-$ARM_VERSION

    export HOST_ROOT="$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64"
    export SYS_ROOT="${HOST_ROOT}/$SYM_LINK_PATH"
    export BIN_DIR="$HOST_ROOT/bin"
    export PATH="$BIN_DIR:$PATH"

    LIB_PATH="${SYS_ROOT}/usr/lib/$ARM_TRIPLE:${SYS_ROOT}/usr/lib/$ARM_TRIPLE/${SDK_VERSION}"
    INC_PATH="${SYS_ROOT}/usr/include"

    export LIBRARY_PATH="$SYS_ROOT/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION"

    LAST_DIR=$PWD

    if [ -d "${SYS_ROOT:?}/" ]; then

        rm -rf "${SYS_ROOT:?}/"
    fi

    mkdir -p "${SYS_ROOT:?}/usr/lib"

    cd "${SYS_ROOT:?}/usr/"

    ln -s "$HOST_ROOT/sysroot/usr/local" "${SYS_ROOT:?}/usr/"

    ln -s "$HOST_ROOT/sysroot/usr/include" "${SYS_ROOT:?}/usr/"

    find "$HOST_ROOT/sysroot/usr/lib/$ARM_NAME_TRIPLE/" -maxdepth 1 -name "*.so" -exec ln -s "{}" "${SYS_ROOT:?}/usr/lib/" \;

    find "$HOST_ROOT/sysroot/usr/lib/$ARM_NAME_TRIPLE/" -maxdepth 1 -name "*.a" -exec ln -s "{}" "${SYS_ROOT:?}/usr/lib/" \;

    find "$HOST_ROOT/sysroot/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION/" -maxdepth 1 -name "*.a" -exec ln -s "{}" "${SYS_ROOT:?}/usr/lib/" \;

    find "$HOST_ROOT/sysroot/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION/" -maxdepth 1 -name "*.so" -exec ln -s "{}" "${SYS_ROOT:?}/usr/lib/" \;

    find "$HOST_ROOT/sysroot/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION/" -maxdepth 1 -name "*.o" -exec ln -s "{}" "${SYS_ROOT:?}/usr/lib/" \;

    cd "$LAST_DIR"

    export BUILD_DIR="build-$ARM_TARGET_ARCH"

    export CC=$ARM_TOOL_TRIPLE-clang
    export CXX=$ARM_TOOL_TRIPLE-clang++
    export LD=llvm-ld
    export AS=llvm-as
    export AR=llvm-ar
    export RANLIB=llvm-ranlib
    export STRIP=llvm-strip
    unset PKG_CONFIG

    ## build mpg123 with autotools (meson port is to much work atm, for this feature)

    LAST_DIR=$PWD

    cd "$SYS_ROOT"

    mkdir -p "build"

    cd build

    wget -q "https://www.mpg123.de/download/mpg123-1.31.3.tar.bz2"

    tar -xf "mpg123-1.31.3.tar.bz2"

    cd "mpg123-1.31.3"

    BUILDYSTEM="cmake"

    if [ $BUILDYSTEM = "autotools" ]; then

        ./configure --prefix="$SYS_ROOT/usr" --oldincludedir="$SYS_ROOT/usr/include" --host="$ARM_NAME_TRIPLE" --with-sysroot="$SYS_ROOT" --with-audio="dummy"

        make

        make install

    else

        cd ports/cmake/

        mkdir -p build

        cd build

        if [ "$ARM_VERSION" = "i686" ]; then
            #cmake .. -DCMAKE_TOOLCHAIN_FILE=linux_i686.toolchain.cmake --install-prefix "$SYS_ROOT/usr" "-DCMAKE_SYSROOT=$SYS_ROOT" -DOUTPUT_MODULES=dummy -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            # cmake --build .

            # cmake --install .
            : # nop, for bash syntax

        else
            cmake .. --install-prefix "$SYS_ROOT/usr" "-DCMAKE_SYSROOT=$SYS_ROOT" -DOUTPUT_MODULES=dummy -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            cmake --build .

            cmake --install .
        fi

    fi

    cd "$LAST_DIR"

    cat <<EOF >"./platforms/crossbuild-android-$ARM_TARGET_ARCH.ini"
[host_machine]
system = 'android'
cpu_family = '$ARCH'
cpu = '$ARM_VERSION'
endian = 'little'

[constants]
android_ndk = '$BIN_DIR'
toolchain = '$BIN_DIR/$ARM_TRIPLE'

[binaries]
c = '$ARM_TOOL_TRIPLE-clang'
cpp = '$ARM_TOOL_TRIPLE-clang++'
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
c_args = ['--sysroot=${SYS_ROOT:?}','-fPIE','-fPIC','--target=$ARM_COMPILER_TRIPLE','-DHAVE_USR_INCLUDE_MALLOC_H','-D_MALLOC_H','-D__BITNESS=$BITNESS']
cpp_args = ['--sysroot=${SYS_ROOT:?}','-fPIE','-fPIC','--target=$ARM_COMPILER_TRIPLE','-D__BITNESS=$BITNESS']
c_link_args = ['-fPIE', '-L$SYS_ROOT/usr/lib']
cpp_link_args = ['-fPIE', '-L$SYS_ROOT/usr/lib']
prefix = '$SYS_ROOT'
libdir = '$LIB_PATH'

[properties]
pkg_config_libdir = '$LIB_PATH'
sys_root = '${SYS_ROOT}'

EOF

    if [ ! -d "$PWD/subprojects/cpu-features" ]; then
        mkdir -p "$PWD/subprojects/cpu-features/src/"
        mkdir -p "$PWD/subprojects/cpu-features/include/"
        ln -s "$BASE_PATH/sources/android/cpufeatures/cpu-features.c" "$PWD/subprojects/cpu-features/src/cpu-features.c"
        ln -s "$BASE_PATH/sources/android/cpufeatures/cpu-features.h" "$PWD/subprojects/cpu-features/include/cpu-features.h"
        cat <<EOF >"$PWD/subprojects/cpu-features/meson.build"
project('cpu-features', 'c')

meson.override_dependency(
    'cpu-features',
    declare_dependency(
        sources: files('src/cpu-features.c'),
        compile_args: [
            '-Wno-declaration-after-statement',
            '-Wno-error',
        ],
        include_directories: include_directories('include'),
    ),
)

EOF
    fi

    export LIBRARY_PATH="$LIBRARY_PATH:usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION:$LIB_PATH"

    meson setup "$BUILD_DIR" \
        "--prefix=$SYS_ROOT" \
        "--includedir=$INC_PATH" \
        "--libdir=usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION" \
        --cross-file "./platforms/crossbuild-android-$ARM_TARGET_ARCH.ini" \
        -Dsdl2:use_hidapi=disabled \
        -Dcpp_args=-DAUDIO_PREFER_MP3

    meson compile -C "$BUILD_DIR"

done

# TODO only copy the supported music ones (atm we need both for i686 flac is needed!)

cp -r ./assets/ platforms/android/app/src/main
