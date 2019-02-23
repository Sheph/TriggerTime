LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libafutil
LOCAL_SRC_FILES := \
    Logger.cpp \
    PNGDecoder.cpp \
    PNGUtils.cpp \
    Utils.cpp \
    EnumTraits.cpp \
    RUBEBase.cpp \
    RUBEBody.cpp \
    RUBEFixture.cpp \
    RUBEImage.cpp \
    RUBEJoint.cpp \
    RUBEWorld.cpp \
    RUBEObject.cpp \
    TPS.cpp \
    GdxP.cpp \
    StreamAppConfig.cpp \
    AndroidAppender.cpp \
    AudioDevice.cpp \
    AudioOggFile.cpp \
    AudioSound.cpp \
    AudioSourceImpl.cpp \
    AudioStream.cpp \
    AudioListener.cpp \
    OAL.cpp

include $(BUILD_STATIC_LIBRARY)
