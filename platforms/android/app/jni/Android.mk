LOCAL_PATH := $(call my-dir)
BUILD_PATH=../../../../build-$(TARGET_ARCH_ABI)
SUBPROJECTS_PATH :=$(BUILD_PATH)/subprojects

# TODO: don't hardcode the versions of the dependencies 

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LIB_PATH := $(SUBPROJECTS_PATH)/SDL2-2.28.5
LOCAL_C_INCLUDES := $(LIB_PATH)/include
LOCAL_SRC_FILES := $(LIB_PATH)/libsdl2.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := freetype
SO_PATH := $(SUBPROJECTS_PATH)/freetype-2.13.2
LOCAL_SRC_FILES :=  $(SO_PATH)/libfreetype.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libpng
SO_PATH := $(SUBPROJECTS_PATH)/libpng-1.6.40
LOCAL_SRC_FILES :=  $(SO_PATH)/libpng16.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf
LIB_PATH := $(SUBPROJECTS_PATH)/SDL2_ttf-2.20.1
LOCAL_SRC_FILES :=  $(LIB_PATH)/libsdl2_ttf.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LIB_PATH := $(SUBPROJECTS_PATH)/SDL2_mixer-2.6.2
LOCAL_SRC_FILES :=  $(LIB_PATH)/libsdl2mixer.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libvorbis
LIB_PATH := $(SUBPROJECTS_PATH)/libvorbis-1.3.7/lib
LOCAL_SRC_FILES :=  $(LIB_PATH)/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libvorbisfile
LIB_PATH := $(SUBPROJECTS_PATH)/libvorbis-1.3.7/lib
LOCAL_SRC_FILES :=  $(LIB_PATH)/libvorbisfile.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libogg
LIB_PATH := $(SUBPROJECTS_PATH)/libogg-1.3.5/src
LOCAL_SRC_FILES :=  $(LIB_PATH)/libogg.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libflac
LIB_PATH := $(SUBPROJECTS_PATH)/flac-1.4.3/src
LOCAL_SRC_FILES :=  $(LIB_PATH)/libFLAC/libFLAC.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_image
LIB_PATH := $(SUBPROJECTS_PATH)/SDL2_image-2.6.3
LOCAL_SRC_FILES :=  $(LIB_PATH)/libsdl2image.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libfmt
LIB_PATH := $(SUBPROJECTS_PATH)/fmt-10.1.1
LOCAL_SRC_FILES :=  $(LIB_PATH)/libfmt.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris
LOCAL_SRC_FILES := $(BUILD_PATH)/liboopetris.so
include $(PREBUILT_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf freetype libpng SDL2_mixer libvorbis libvorbisfile libogg libflac SDL2_image libfmt liboopetris
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid
LOCAL_LDFLAGS := -Wl,--no-undefined
include $(BUILD_SHARED_LIBRARY)
