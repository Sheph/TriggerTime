LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libogg
LOCAL_SRC_FILES := \
    bitwise.c \
    framing.c

include $(BUILD_STATIC_LIBRARY)
