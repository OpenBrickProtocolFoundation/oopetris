LOCAL_PATH := $(call my-dir)
BUILD_PATH=$(LOCAL_PATH)/../../../../build-$(TARGET_ARCH_ABI)
SUBPROJECTS_PATH :=$(BUILD_PATH)/subprojects

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libfreetype.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libpng
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libpng16.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2_ttf.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2mixer.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libvorbis
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libvorbis.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libvorbisfile
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libvorbisfile.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libogg
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libogg.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libflac
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libFLAC.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_image
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libsdl2image.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libfmt
LOCAL_SRC_FILES := $(shell find "${SUBPROJECTS_PATH}" -name libfmt.so)
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris_core
LIB_PATH := $(BUILD_PATH)/src/libs/core
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris_core.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris_recordings
LIB_PATH := $(BUILD_PATH)/src/libs/recordings
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris_recordings.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris_graphics
LIB_PATH := $(BUILD_PATH)/src
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris_graphics.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris
LIB_PATH := $(BUILD_PATH)/src/executables
LOCAL_SRC_FILES := $(LIB_PATH)/liboopetris.so
include $(PREBUILT_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf freetype libpng SDL2_mixer libvorbis libvorbisfile libogg libflac SDL2_image libfmt liboopetris_core liboopetris_recordings liboopetris_graphics liboopetris
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid
LOCAL_LDFLAGS := -Wl,--no-undefined
include $(BUILD_SHARED_LIBRARY)
