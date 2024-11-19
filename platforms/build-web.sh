#!/usr/bin/env bash

set -e

if [ ! -d "toolchains" ]; then
    mkdir -p toolchains
fi

export EMSCRIPTEN_ROOT="$(pwd)/toolchains/emsdk"

if [ ! -d "$EMSCRIPTEN_ROOT" ]; then
    git clone https://github.com/emscripten-core/emsdk.git "$EMSCRIPTEN_ROOT"
fi

"$EMSCRIPTEN_ROOT/emsdk" install latest
"$EMSCRIPTEN_ROOT/emsdk" activate latest

EMSCRIPTEN_UPSTREAM_ROOT="$EMSCRIPTEN_ROOT/upstream/emscripten"

EMSCRIPTEN_PATCH_FILE="$EMSCRIPTEN_UPSTREAM_ROOT/.patched_manually.meta"

PATCH_DIR="platforms/emscripten"

if ! [ -e "$EMSCRIPTEN_PATCH_FILE" ]; then
    ##TODO: upstream those patches
    # see: https://github.com/emscripten-core/emscripten/pull/18379
    # and: https://github.com/emscripten-core/emscripten/pull/22946

    git apply --unsafe-paths -p1 --directory="$EMSCRIPTEN_UPSTREAM_ROOT" "$PATCH_DIR/sdl2_image_port.diff"

    touch "$EMSCRIPTEN_PATCH_FILE"
fi

# git apply path

# shellcheck disable=SC1091
EMSDK_QUIET=1 source "$EMSCRIPTEN_ROOT/emsdk_env.sh" >/dev/null

PTHREAD_POOL_SIZE="8"

## build theneeded dependencies
embuilder build sdl2-mt harfbuzz-mt freetype zlib sdl2_ttf mpg123 "sdl2_mixer-mp3-mt" libpng-mt "sdl2_image:formats=png,svg:mt=1" icu-mt

export EMSCRIPTEN_SYS_ROOT="$EMSCRIPTEN_UPSTREAM_ROOT/cache/sysroot"

EMSCRIPTEN_SYS_LIB_DIR="$EMSCRIPTEN_SYS_ROOT/lib/wasm32-emscripten"
EMSCRIPTEN_SYS_PKGCONFIG_DIR="$EMSCRIPTEN_SYS_ROOT/lib/pkgconfig"

export CC="emcc"
export CXX="em++"
export AR="emar"
export RANLIB="emranlib"
export STRIP="emstrip"
export NM="emnm"

EMSCRIPTEN_PORT_BUILD_DIR="$EMSCRIPTEN_UPSTREAM_ROOT/cache/ports"

BUILD_DIR_FFMPEG="build-ffmpeg"

BUILD_FFMPEG_FILE="$EMSCRIPTEN_PORT_BUILD_DIR/$BUILD_DIR_FFMPEG/build_successfull.meta"

# build the ffmpeg dependencies
# taken from: https://dev.to/alfg/ffmpeg-webassembly-2cbl
# modifed to fit the style of this project + some manual modifications
if [ "$COMPILE_TYPE" == "complete_rebuild" ] || ! [ -e "$BUILD_FFMPEG_FILE" ]; then

    LAST_DIR="$PWD"

    cd "$EMSCRIPTEN_PORT_BUILD_DIR"

    mkdir -p "$BUILD_DIR_FFMPEG"

    cd "$BUILD_DIR_FFMPEG"

    LIBX264_DIR="x264-src"

    if ! [ -e "$LIBX264_DIR" ]; then

        LIBX264_DIR_VERSION="20191217-2245-stable"

        wget "https://download.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-${LIBX264_DIR_VERSION}.tar.bz2"

        tar xvfj "x264-snapshot-${LIBX264_DIR_VERSION}.tar.bz2"

        mv "x264-snapshot-${LIBX264_DIR_VERSION}" "$LIBX264_DIR"
    fi

    cd "$LIBX264_DIR"

    BUILD_LIBX264_FILE="build_successfull.meta"

    if ! [ -e "$BUILD_LIBX264_FILE" ]; then

        emconfigure ./configure \
            --enable-static \
            --disable-cli \
            --disable-asm \
            --extra-cflags="-sUSE_PTHREADS=1" \
            --host=i686-gnu \
            --sysroot="$EMSCRIPTEN_SYS_ROOT" \
            --prefix="$EMSCRIPTEN_SYS_ROOT" \
            --libdir="$EMSCRIPTEN_SYS_LIB_DIR" \
            --pkgconfigdir="$EMSCRIPTEN_SYS_PKGCONFIG_DIR"

        emmake make -j

        emmake make install

        touch "$BUILD_LIBX264_FILE"

    fi

    cd ..

    FFMPEG_CLONE_DIR="ffmpeg-src"

    GIT_FFMPEG_TAG="n7.1"

    if ! [ -e "$FFMPEG_CLONE_DIR" ]; then

        git clone https://github.com/FFmpeg/FFmpeg "$FFMPEG_CLONE_DIR"

        cd "$FFMPEG_CLONE_DIR"

        git checkout "$GIT_FFMPEG_TAG"

    else
        cd "$FFMPEG_CLONE_DIR"

        git checkout "$GIT_FFMPEG_TAG"

    fi

    FFMPEG_COMMON_FLAGS="-pthread -sUSE_PTHREADS=1"

    FFMPEG_LINK_FLAGS="$COMMON_FLAGS -sWASM=1 -sALLOW_MEMORY_GROWTH=1 -sASSERTIONS=1 -sERROR_ON_UNDEFINED_SYMBOLS=1 -sPTHREAD_POOL_SIZE=$PTHREAD_POOL_SIZE"

    ##TODO: add --disable-debug, in release mode

    # Configure and build FFmpeg with emscripten.
    # Disable all programs and only enable features we will use.
    # https://github.com/FFmpeg/FFmpeg/blob/master/configure
    emconfigure ./configure \
        --disable-asm \
        --disable-x86asm \
        --disable-inline-asm \
        --disable-stripping \
        --target-os=none \
        --arch=x86_32 \
        --enable-cross-compile \
        --disable-doc \
        --disable-programs \
        --disable-sdl2 \
        --disable-all \
        --enable-avcodec \
        --enable-avformat \
        --enable-avfilter \
        --enable-avdevice \
        --enable-avutil \
        --enable-swresample \
        --enable-swscale \
        --enable-filters \
        --enable-protocol="file,pipe,tcp" \
        --enable-decoder="rawvideo" \
        --enable-encoder="libx264" \
        --enable-demuxer="rawvideo" \
        --enable-muxer="mp4" \
        --enable-gpl \
        --enable-libx264 \
        --extra-cflags="$FFMPEG_COMMON_FLAGS" \
        --extra-cxxflags="$FFMPEG_COMMON_FLAGS" \
        --extra-ldflags="$FFMPEG_LINK_FLAGS" \
        --sysroot="$EMSCRIPTEN_SYS_ROOT" \
        --prefix="$EMSCRIPTEN_SYS_ROOT" \
        --libdir="$EMSCRIPTEN_SYS_LIB_DIR" \
        --pkgconfigdir="$EMSCRIPTEN_SYS_PKGCONFIG_DIR" \
        --nm="$NM" \
        --ar="$AR" \
        --cc="$CC" \
        --cxx="$CXX" \
        --objcc="$CC" \
        --dep-cc="$CC"

    emmake make -j

    emmake make install

    touch "$BUILD_FFMPEG_FILE"

    cd "$LAST_DIR"

fi

export BUILD_DIR="build-web"

export ARCH="wasm32"
export CPU_ARCH="wasm32"
export ENDIANESS="little"

export ROMFS="platforms/romfs"

#TODO: differentiate between release and debug mode, disable -sASSERTIONS and other debbug utilities
export PACKAGE_FLAGS="'--use-port=sdl2', '--use-port=harfbuzz', '--use-port=freetype', '--use-port=zlib', '--use-port=sdl2_ttf', '--use-port=mpg123', '--use-port=sdl2_mixer', '-sSDL2_MIXER_FORMATS=[\"mp3\"]','--use-port=libpng', '--use-port=sdl2_image','-sSDL2_IMAGE_FORMATS=[\"png\",\"svg\"]', '--use-port=icu'"

export COMMON_FLAGS="'-fexceptions', '-pthread', '-sUSE_PTHREADS=1', '-sEXCEPTION_CATCHING_ALLOWED=[..]', $PACKAGE_FLAGS"

# TODO see if ALLOW_MEMORY_GROWTH is needed, but if we load ttf's and music it likely is and we don't have to debug OOm crashes, that aren't handled by some third party library, which is painful
export LINK_FLAGS="$COMMON_FLAGS, '-sEXPORT_ALL=1', '-sUSE_WEBGPU=1', '-sWASM=1', '-sALLOW_MEMORY_GROWTH=1', '-sASSERTIONS=1','-sERROR_ON_UNDEFINED_SYMBOLS=1', '-sFETCH=1', '-sEXIT_RUNTIME=1', '-sPTHREAD_POOL_SIZE=$PTHREAD_POOL_SIZE','-lidbfs.js'"
export COMPILE_FLAGS="$COMMON_FLAGS ,'-DAUDIO_PREFER_MP3'"

export CROSS_FILE="./platforms/crossbuild-web.ini"

cat <<EOF >"$CROSS_FILE"
[host_machine]
system = 'emscripten'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[target_machine]
system = 'emscripten'
cpu_family = '$ARCH'
cpu = '$CPU_ARCH'
endian = '$ENDIANESS'

[constants]
emscripten_root = '$(pwd)/emsdk'

[binaries]
c = '$CC'
cpp = '$CXX'
ar      = '$AR'
ranlib  = '$RANLIB'
strip   = '$STRIP'
nm = '$NM'

pkg-config = ['emmake', 'pkg-config']
cmake = ['emmake', 'cmake']
sdl2-config = ['emconfigure', 'sdl2-config']

exe_wrapper = '$EMSDK_NODE'

[built-in options]
c_std = 'c11'
cpp_std = 'c++23'
c_args = [$COMPILE_FLAGS]
cpp_args = [$COMPILE_FLAGS]
c_link_args = [$LINK_FLAGS]
cpp_link_args = [$LINK_FLAGS]

[properties]
needs_exe_wrapper = true
sys_root = '$EMSCRIPTEN_SYS_ROOT'

APP_ROMFS='$ROMFS/assets/'

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
        -Ddefault_library=static \
        -Dtests=false \
        -Duse_embedded_ffmpeg=enabled

fi

meson compile -C "$BUILD_DIR"

if [ -n "$ENABLE_TESTING" ]; then

    meson test -C "$BUILD_DIR"

fi
