LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpoly2tri
LOCAL_C_INCLUDES := ../include/common ../include/sweep
LOCAL_SRC_FILES := \
    common/shapes.cc \
    sweep/advancing_front.cc \
    sweep/cdt.cc \
    sweep/sweep.cc \
    sweep/sweep_context.cc

include $(BUILD_STATIC_LIBRARY)
