LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/anapi/Anapi.mk

include $(CLEAR_VARS)

LOCAL_MODULE		:= simple-anapi-app

LOCAL_SRC_FILES		:= simple.cpp

LOCAL_STATIC_LIBRARIES	:= anapi
LOCAL_CFLAGS		:= -std=c++11
LOCAL_LDLIBS		:= -llog -landroid
LOCAL_CPP_FEATURES	+= exceptions rtti

include $(BUILD_SHARED_LIBRARY)
