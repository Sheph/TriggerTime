LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libjsoncpp
LOCAL_SRC_FILES := \
    jsoncpp.cpp

include $(BUILD_STATIC_LIBRARY)
