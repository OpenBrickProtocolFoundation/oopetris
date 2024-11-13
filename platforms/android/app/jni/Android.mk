LOCAL_PATH := $(call my-dir)
BUILD_PATH=$(LOCAL_PATH)/../../../../build-$(TARGET_ARCH_ABI)
SUBPROJECTS_PATH :=$(BUILD_PATH)/subprojects

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libSDL2.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libfreetype.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := png16
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libpng16.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := sdl2_ttf
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2_ttf.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := sdl2_mixer
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2mixer.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := vorbis
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libvorbis.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := vorbisfile
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libvorbisfile.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := ogg
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libogg.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := sdl2_image
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2image.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := fmt
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libfmt.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := keyutils
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libkeyutils.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libavutil
LOCAL_SRC_FILES := $(shell meson introspect --dependencies "${BUILD_PATH}" | jq  -r ".[] | select(.name==\"libavutil\") | .link_args | .[]")
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libavcodec
LOCAL_SRC_FILES := $(shell meson introspect --dependencies "${BUILD_PATH}" | jq  -r ".[] | select(.name==\"libavcodec\") | .link_args | .[]")
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libavformat
LOCAL_SRC_FILES := $(shell meson introspect --dependencies "${BUILD_PATH}" | jq  -r ".[] | select(.name==\"libavformat\") | .link_args | .[]")
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libavfilter
LOCAL_SRC_FILES := $(shell meson introspect --dependencies "${BUILD_PATH}" | jq  -r ".[] | select(.name==\"libavfilter\") | .link_args | .[]")
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libswscale
LOCAL_SRC_FILES := $(shell meson introspect --dependencies "${BUILD_PATH}" | jq  -r ".[] | select(.name==\"libswscale\") | .link_args | .[]")
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libswresample
LOCAL_SRC_FILES := $(shell meson introspect --dependencies "${BUILD_PATH}" | jq  -r ".[] | select(.name==\"libswresample\") | .link_args | .[]")
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := oopetris_core
LIB_PATH := $(BUILD_PATH)/src/libs/core
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris_core.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := oopetris_recordings
LIB_PATH := $(BUILD_PATH)/src/libs/recordings
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris_recordings.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := oopetris_graphics
LIB_PATH := $(BUILD_PATH)/src
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris_graphics.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := oopetris
LIB_PATH := $(BUILD_PATH)/src/executables
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris.so
include $(PREBUILT_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_SHARED_LIBRARIES := SDL2 sdl2_ttf freetype png16 sdl2_mixer vorbis vorbisfile ogg sdl2_image fmt keyutils oopetris_core oopetris_recordings oopetris_graphics oopetris libavutil libavcodec libavformat libavfilter libswscale libswresample
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid
LOCAL_LDFLAGS := -Wl,--no-undefined
include $(BUILD_SHARED_LIBRARY)
