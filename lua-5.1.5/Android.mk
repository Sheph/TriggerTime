LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblua
LOCAL_CFLAGS := -DLUA_USE_LINUX
LOCAL_SRC_FILES := \
    lapi.cpp \
    lcode.cpp \
    ldo.cpp \
    lgc.cpp \
    llex.cpp \
    loadlib.cpp \
    loslib.cpp \
    lstring.cpp \
    ltablib.cpp \
    lzio.cpp \
    lauxlib.cpp \
    ldblib.cpp \
    ldump.cpp \
    linit.cpp \
    lmathlib.cpp \
    lobject.cpp \
    lparser.cpp \
    lstrlib.cpp \
    ltm.cpp \
    lundump.cpp \
    lbaselib.cpp \
    ldebug.cpp \
    lfunc.cpp \
    liolib.cpp \
    lmem.cpp \
    lopcodes.cpp \
    lstate.cpp \
    ltable.cpp \
    lvm.cpp

include $(BUILD_STATIC_LIBRARY)
