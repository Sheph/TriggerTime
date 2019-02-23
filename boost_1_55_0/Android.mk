LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libboost
LOCAL_CFLAGS := -DBOOST_SYSTEM_STATIC_LINK=1 -DBOOST_THREAD_BUILD_LIB=1 -DBOOST_THREAD_POSIX
LOCAL_SRC_FILES := \
    error_code.cpp \
    pthread/thread.cpp \
    pthread/once.cpp \
    android/lockpool.cpp \
    future.cpp

include $(BUILD_STATIC_LIBRARY)
