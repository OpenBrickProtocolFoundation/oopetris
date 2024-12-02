#!/usr/bin/env bash

set -e

if [ ! -d "toolchains" ]; then
    mkdir -p toolchains
fi

export NDK_VER_DOWNLOAD="r29-beta1"
export NDK_VER_DESC="r29-beta1"

export BASE_PATH="$PWD/toolchains/android-ndk-$NDK_VER_DESC"
export ANDROID_NDK_HOME="$BASE_PATH"
export ANDROID_NDK="$BASE_PATH"

if [ ! -d "$BASE_PATH" ]; then

    cd toolchains

    if [ ! -e "android-ndk-$NDK_VER_DOWNLOAD-linux.zip" ]; then

        wget -q "https://dl.google.com/android/repository/android-ndk-$NDK_VER_DOWNLOAD-linux.zip"
    fi
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

export ARCH_KEYS_INDEX=("${!ARCH_KEYS[@]}")

## options: "smart, complete_rebuild"
export COMPILE_TYPE="smart"

export BUILDTYPE="debug"

if [ "$#" -eq 0 ]; then
    # nothing
    echo "Using all architectures"
elif [ "$#" -eq 1 ] || [ "$#" -eq 2 ] || [ "$#" -eq 3 ]; then
    ARCH=$1

    FOUND=""

    for INDEX in "${!ARCH_KEYS[@]}"; do
        if [[ "$ARCH" == "${ARCH_KEYS[$INDEX]}" ]]; then
            FOUND="$INDEX"
        fi
    done

    if [ -z "$FOUND" ]; then
        echo "Invalid arch: '${ARCH}', supported archs are:" "${ARCH_KEYS[@]}"
        exit 2
    fi

    ARCH_KEYS_INDEX=("$FOUND")

    if [ "$#" -eq 2 ]; then
        COMPILE_TYPE="$2"
    elif [ "$#" -eq 3 ]; then
        COMPILE_TYPE="$2"
        BUILDTYPE="$3"
    fi

else
    echo "Too many arguments given, expected 1 ,2 or 3"
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

for INDEX in "${ARCH_KEYS_INDEX[@]}"; do
    export KEY=${ARCH_KEYS[$INDEX]}

    RAW_JSON=$(jq '.[$KEY]' -M -r -c --arg KEY "$KEY" "$BASE_PATH/meta/abis.json")

    BITNESS=$(echo "$RAW_JSON" | jq -M -r -c '."bitness"') || true
    ARCH=$(echo "$RAW_JSON" | jq -M -r -c '."arch"')
    ARCH_VERSION=$(echo "$RAW_JSON" | jq -M -r -c '."proc"' | tr -d "-")
    ARM_NAME_TRIPLE=$(echo "$RAW_JSON" | jq -M -r -c '."triple"')
    ARM_TARGET_ARCH=$KEY
    ARM_TRIPLE=$ARM_NAME_TRIPLE$SDK_VERSION
    ARM_COMPILER_TRIPLE=$(echo "$RAW_JSON" | jq -M -r -c '."llvm_triple"')
    ARM_TOOL_TRIPLE=$(echo "$ARM_NAME_TRIPLE$SDK_VERSION" | sed s/$ARCH/$ARCH_VERSION/)

    export SYM_LINK_PATH=sym-$ARCH_VERSION

    export HOST_ROOT="$BASE_PATH/toolchains/llvm/prebuilt/linux-x86_64"
    export SYS_ROOT="${HOST_ROOT}/$SYM_LINK_PATH/sysroot"
    export BIN_DIR="$HOST_ROOT/bin"
    export PATH="$BIN_DIR:$PATH"

    export LIB_PATH="${SYS_ROOT}/usr/lib/$ARM_TRIPLE:${SYS_ROOT}/usr/lib/$ARM_TRIPLE/${SDK_VERSION}"
    export INC_PATH="${SYS_ROOT}/usr/include"
    export PKG_CONFIG_PATH="${SYS_ROOT}/usr/lib/pkgconfig/"

    export LIBRARY_PATH="$SYS_ROOT/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION"

    if [ "$COMPILE_TYPE" == "complete_rebuild" ] || ! [ -e "$SYS_ROOT" ]; then

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

        # TODO: remove this temporary fix:
        # see: https://github.com/android/ndk/issues/2107
        if [ "$ARCH_VERSION" = "armv7a" ]; then
            sed -i -e 's/asm(/__asm__(/g' "$HOST_ROOT/sysroot/usr/include/arm-linux-androideabi/asm/swab.h"
        elif [ "$ARCH_VERSION" = "i686" ]; then
            sed -i -e 's/asm(/__asm__(/g' "$HOST_ROOT/sysroot/usr/include/i686-linux-android/asm/swab.h"
        elif [ "$ARCH_VERSION" = "x86_64" ]; then
            sed -i -e 's/asm(/__asm__(/g' "$HOST_ROOT/sysroot/usr/include/x86_64-linux-android/asm/swab.h"
        fi

        cd "$LAST_DIR"

    fi

    export BUILD_DIR="build-$ARM_TARGET_ARCH"

    export CC="$ARM_TOOL_TRIPLE-clang"
    export CXX="$ARM_TOOL_TRIPLE-clang++"
    export LD="llvm-ld"
    export AS="llvm-as"
    export AR="llvm-ar"
    export RANLIB="llvm-ranlib"
    export STRIP="llvm-strip"
    export OBJCOPY="llvm-objcop"
    export LLVM_CONFIG="llvm-config"
    unset PKG_CONFIG

    ## BUILD dependencies not buildable with meson (to complicated to port)

    ## build mpg123 with cmake (meson port is to much work atm, for this feature)

    LAST_DIR="$PWD"

    cd "$SYS_ROOT"

    BUILD_DIR_MPG123="build-mpg123"

    BUILD_MPG123_FILE="$SYS_ROOT/$BUILD_DIR_MPG123/build_successfull.meta"

    if [ "$COMPILE_TYPE" == "complete_rebuild" ] || ! [ -e "$BUILD_MPG123_FILE" ]; then

        mkdir -p "$BUILD_DIR_MPG123"

        cd "$BUILD_DIR_MPG123"

        MPG123_VERSION="1.32.9"

        if [ ! -e "mpg123-$MPG123_VERSION.tar.bz2" ]; then
            wget -q "https://www.mpg123.de/download/mpg123-$MPG123_VERSION.tar.bz2"
        fi

        if [ ! -d "mpg123-$MPG123_VERSION" ]; then
            tar -xf "mpg123-$MPG123_VERSION.tar.bz2"
        fi

        cd "mpg123-$MPG123_VERSION"

        cd ports/cmake/

        mkdir -p "$BUILD_DIR_MPG123"

        cd "$BUILD_DIR_MPG123"

        export MPG123_ANDROID_SSE_ENABLED="ON"

        if [ "$ARCH_VERSION" = "i686" ]; then
            MPG123_ANDROID_SSE_ENABLED="OFF"
        fi

        cmake .. --install-prefix "$SYS_ROOT/usr" "-DCMAKE_SYSROOT=$SYS_ROOT" -DOUTPUT_MODULES=dummy -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            "-DCMAKE_SYSTEM_NAME=Android" \
            "-DCMAKE_SYSTEM_VERSION=$SDK_VERSION" \
            "-DCMAKE_ANDROID_ARCH_ABI=$KEY" \
            "-DCMAKE_ANDROID_NDK=$ANDROID_NDK" \
            "-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang" \
            "-DWITH_SSE=$MPG123_ANDROID_SSE_ENABLED"

        cmake --build .

        cmake --install .

        touch "$BUILD_MPG123_FILE"

    fi

    cd "$LAST_DIR"

    ## build flac with cmake (meson port doesn't work for 32 bits machines atm) (we need to check for fseeko and ftello correctly in there)

    LAST_DIR="$PWD"

    cd "$SYS_ROOT"

    BUILD_DIR_FLAC="build-flac"

    BUILD_FLAC_FILE="$SYS_ROOT/$BUILD_DIR_FLAC/build_successfull.meta"

    if [ "$COMPILE_TYPE" == "complete_rebuild" ] || ! [ -e "$BUILD_FLAC_FILE" ]; then

        mkdir -p "$BUILD_DIR_FLAC"

        cd "$BUILD_DIR_FLAC"

        FLAC_VERSION="1.4.3"

        if [ ! -e "flac-$FLAC_VERSION.tar.xz" ]; then
            wget -q "https://github.com/xiph/flac/releases/download/$FLAC_VERSION/flac-$FLAC_VERSION.tar.xz"
        fi

        if [ ! -d "flac-$FLAC_VERSION" ]; then
            tar -xf "flac-$FLAC_VERSION.tar.xz"
        fi

        cd "flac-$FLAC_VERSION"

        mkdir -p "$BUILD_DIR_FLAC"

        cd "$BUILD_DIR_FLAC"

        cmake .. --install-prefix "$SYS_ROOT/usr" "-DCMAKE_SYSROOT=$SYS_ROOT" -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
            "-DCMAKE_SYSTEM_NAME=Android" \
            "-DCMAKE_SYSTEM_VERSION=$SDK_VERSION" \
            "-DCMAKE_ANDROID_ARCH_ABI=$KEY" \
            "-DCMAKE_ANDROID_NDK=$ANDROID_NDK" \
            "-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang" \
            -DBUILD_PROGRAMS=OFF \
            -DBUILD_EXAMPLES=OFF \
            -DBUILD_TESTING=OFF \
            -DBUILD_CXXLIBS=OFF \
            -DBUILD_DOCS=OFF \
            -DWITH_OGG=OFF \
            -DBUILD_SHARED_LIBS=OFF \
            -DINSTALL_PKGCONFIG_MODULES=ON

        cmake --build .

        cmake --install .

        touch "$BUILD_FLAC_FILE"

    fi

    cd "$LAST_DIR"

    ## build openssl with make (meson port is to much work atm, for this feature)

    LAST_DIR="$PWD"

    cd "$SYS_ROOT"

    BUILD_DIR_OPENSSL="build-openssl"

    BUILD_OPENSSL_FILE="$SYS_ROOT/$BUILD_DIR_OPENSSL/build_successfull.meta"

    if [ "$COMPILE_TYPE" == "complete_rebuild" ] || ! [ -e "$BUILD_OPENSSL_FILE" ]; then

        mkdir -p "$BUILD_DIR_OPENSSL"

        cd "$BUILD_DIR_OPENSSL"

        OPENSSL_VERSION="3.4.0"

        if [ ! -e "openssl-$OPENSSL_VERSION.tar.gz" ]; then
            wget -q "https://github.com/openssl/openssl/releases/download/openssl-$OPENSSL_VERSION/openssl-$OPENSSL_VERSION.tar.gz"
        fi

        if [ ! -d "openssl-$OPENSSL_VERSION" ]; then
            tar -xzf "openssl-$OPENSSL_VERSION.tar.gz"
        fi

        cd "openssl-$OPENSSL_VERSION"

        OPENSSL_TARGET_ARCH="android-$ARCH"

        export ANDROID_NDK_ROOT="$ANDROID_NDK_HOME"

        if [ "$ARCH_VERSION" = "armv7a" ]; then

            ./Configure --prefix="$SYS_ROOT/usr" no-asm no-tests no-shared "$OPENSSL_TARGET_ARCH" "-D__ANDROID_API__=$SDK_VERSION"
        else
            ./Configure --prefix="$SYS_ROOT/usr" no-tests no-shared "$OPENSSL_TARGET_ARCH" "-D__ANDROID_API__=$SDK_VERSION"
        fi

        make clean

        if [ "$ARCH_VERSION" = "armv7-a" ]; then

            # fix an compile time error since openssl 3.1.0 >
            # see https://github.com/android/ndk/issues/1992
            # see https://github.com/openssl/openssl/pull/22181
            # Apply patch that fixes the armcap instruction

            # sed -e '/[.]hidden.*OPENSSL_armcap_P/d; /[.]extern.*OPENSSL_armcap_P/ {p; s/extern/hidden/ }' -i -- crypto/*arm*pl crypto/*/asm/*arm*pl
            sed -E -i '' -e '/[.]hidden.*OPENSSL_armcap_P/d' -e '/[.]extern.*OPENSSL_armcap_P/ {p; s/extern/hidden/; }' crypto/*arm*pl crypto/*/asm/*arm*pl

        fi

        make -j build_sw

        make install_sw

        touch "$BUILD_OPENSSL_FILE"

    fi

    cd "$LAST_DIR"

    ## build ffmpeg for android (using https://github.com/Javernaut/ffmpeg-android-maker)

    LAST_DIR="$PWD"

    cd "$SYS_ROOT"

    BUILD_DIR_FFMPEG="build-ffmpeg"

    BUILD_FFMPEG_FILE="$SYS_ROOT/$BUILD_DIR_FFMPEG/build_successfull.meta"

    if [ "$COMPILE_TYPE" == "complete_rebuild" ] || ! [ -e "$BUILD_FFMPEG_FILE" ]; then

        mkdir -p "$BUILD_DIR_FFMPEG"

        cd "$BUILD_DIR_FFMPEG"

        FFMPEG_MAKER_DIR="maker"

        if ! [ -e "$FFMPEG_MAKER_DIR" ]; then

            git clone https://github.com/Javernaut/ffmpeg-android-maker.git "$FFMPEG_MAKER_DIR"

            cd "$FFMPEG_MAKER_DIR"
        else
            cd "$FFMPEG_MAKER_DIR"

            git pull

        fi

        ./ffmpeg-android-maker.sh "--target-abis=$ARCH" "--android-api-level=$SDK_VERSION" --enable-libx264

        FFMPEG_MAKER_OUTPUT_DIR="output"

        find "$FFMPEG_MAKER_OUTPUT_DIR/include/" -maxdepth 3 -mindepth 2 -type d -exec cp -r {} "$SYS_ROOT/usr/include/" \;

        find "$FFMPEG_MAKER_OUTPUT_DIR/lib/" -type f -exec cp -r {} "$SYS_ROOT/usr/lib/" \;

        find "build/" -maxdepth 5 -mindepth 4 -type f -name "*.pc" -exec cp -r {} "$SYS_ROOT/usr/lib/pkgconfig/" \;

        touch "$BUILD_FFMPEG_FILE"

    fi

    cd "$LAST_DIR"

    ## END of manual build of dependencies

    MESON_CPU_FAMILY=$ARCH

    ## this is a flaw in the abis.json, everything is labelled with aarch64 and not arm64, but the "arch" json value is wrong, and meson (https://mesonbuild.com/Reference-tables.html#cpu-families) only knows aarch64!
    if [[ $MESON_CPU_FAMILY = "arm64" ]]; then
        MESON_CPU_FAMILY="aarch64"
    fi

    export COMPILE_FLAGS="'--sysroot=${SYS_ROOT:?}','-fPIE','-fPIC','--target=$ARM_COMPILER_TRIPLE','-D__ANDROID_API__=$SDK_VERSION', '-DBITNESS=$BITNESS','-DAUDIO_PREFER_MP3'"

    export LINK_FLAGS="'-fPIE','-L$SYS_ROOT/usr/lib'"

    cat <<EOF >"./platforms/crossbuild-android-$ARM_TARGET_ARCH.ini"
[host_machine]
system = 'android'
cpu_family = '$MESON_CPU_FAMILY'
cpu = '$ARCH_VERSION'
endian = 'little'

[constants]
android_ndk = '$BIN_DIR'
toolchain = '$BIN_DIR/$ARM_TRIPLE'

[binaries]
c = '$CC'
cpp = '$CXX'
c_ld = 'lld'
cpp_ld = 'lld'
ar      = '$AR'
as      = '$AS'
ranlib  = '$RANLIB'
strip   = '$STRIP'
objcopy = '$OBJCOPY'
pkg-config = 'pkg-config'
llvm-config = '$LLVM_CONFIG'

[built-in options]
c_std = 'gnu11'
cpp_std = 'c++23'
c_args = [$COMPILE_FLAGS]
cpp_args = [$COMPILE_FLAGS]            
c_link_args = [$LINK_FLAGS]
cpp_link_args = [$LINK_FLAGS]

prefix = '$SYS_ROOT'
libdir = '$LIB_PATH'

[properties]
pkg_config_libdir = '$PKG_CONFIG_PATH'
sys_root = '${SYS_ROOT}'

EOF

    CPU_FUTURES_ROOT="$PWD/subprojects/cpu-features"

    if [ ! -d "$CPU_FUTURES_ROOT" ]; then
        mkdir -p "$CPU_FUTURES_ROOT/src/"
        mkdir -p "$CPU_FUTURES_ROOT/include/"
        ln -s "$BASE_PATH/sources/android/cpufeatures/cpu-features.c" "$CPU_FUTURES_ROOT/src/cpu-features.c"
        ln -s "$BASE_PATH/sources/android/cpufeatures/cpu-features.h" "$CPU_FUTURES_ROOT/include/cpu-features.h"
        cat <<EOF >"$CPU_FUTURES_ROOT/meson.build"
project('cpu-features','c')

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

    export LIBRARY_PATH="$LIBRARY_PATH:$SYS_ROOT/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION:$LIB_PATH"

    if [ "$COMPILE_TYPE" == "complete_rebuild" ] || [ ! -e "$BUILD_DIR" ]; then

        # TODO: enbale hidapi, by not dependening on libusb, that is not availbale on android
        meson setup "$BUILD_DIR" \
            "--prefix=$SYS_ROOT" \
            "--wipe" \
            "--includedir=$INC_PATH" \
            "--libdir=$SYS_ROOT/usr/lib/$ARM_NAME_TRIPLE/$SDK_VERSION" \
            --cross-file "./platforms/crossbuild-android-$ARM_TARGET_ARCH.ini" \
            "-Dbuildtype=$BUILDTYPE" \
            -Dsdl2:use_hidapi=enabled \
            -Dclang_libcpp=disabled \
            -Duse_embedded_ffmpeg=enabled

    fi

    meson compile -C "$BUILD_DIR"

    echo -e "Sucesfully build for android platform ${ARCH}\n"

done

# TODO only copy the supported music ones (atm we need both for i686 flac is needed!)

cp -r ./assets/ platforms/android/app/src/main

# copy icons to correct dir

DESC_ARRAY=(m h xh xxh xxxh)
RES_ARRAY=(48 72 96 144 192)

for IDX in "${!DESC_ARRAY[@]}"; do

    DESC="${DESC_ARRAY[$IDX]}"
    RES="${RES_ARRAY[$IDX]}"

    export DESC_DIR="platforms/android/app/src/main/res/mipmap-${DESC}dpi"

    mkdir -p "$DESC_DIR"
    cp -r "./assets/icon/${RES}x${RES}.png" "${DESC_DIR}/ic_launcher.png"

done
