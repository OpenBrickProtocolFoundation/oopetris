LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := ../../../../build/subprojects/SDL2-2.26.0/libsdl2.so
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf
LOCAL_SRC_FILES := ../../../../build/subprojects/SDL2_ttf-2.20.1/libsdl2_ttf.so
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := liboopetris
LOCAL_SRC_FILES := ../../../../build/liboopetris.so
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf liboopetris

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)



