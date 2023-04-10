LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
SDL_PATH := ../../../../build-$(TARGET_ARCH_ABI)/subprojects/SDL2-2.26.0
LOCAL_C_INCLUDES := $(SDL_PATH)/include
LOCAL_SRC_FILES := $(SDL_PATH)/libsdl2.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := freetype
SO_PATH := ../../../../build-$(TARGET_ARCH_ABI)/subprojects/freetype-2.12.1
LOCAL_SRC_FILES :=  $(SO_PATH)/libfreetype.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libpng
SO_PATH := ../../../../build-$(TARGET_ARCH_ABI)/subprojects/libpng-1.6.39
LOCAL_SRC_FILES :=  $(SO_PATH)/libpng16.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf
SDL_PATH := ../../../../build-$(TARGET_ARCH_ABI)/subprojects/SDL2_ttf-2.20.1
LOCAL_SRC_FILES :=  $(SDL_PATH)/libsdl2_ttf.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris
LOCAL_SRC_FILES := ../../../../build-$(TARGET_ARCH_ABI)/liboopetris.so
include $(PREBUILT_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf freetype libpng liboopetris
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid
LOCAL_LDFLAGS := -Wl,--no-undefined
TARGET_ARCH_ABI= arm64-v8a
include $(BUILD_SHARED_LIBRARY)
