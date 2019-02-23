LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libopenal-soft
LOCAL_LDLIBS := -lOpenSLES -llog
LOCAL_C_INCLUDES := ../openal-soft ../openal-soft/include ../openal-soft/OpenAL32/Include ../openal-soft/Alc
LOCAL_CFLAGS := -Drestrict=__restrict -DAL_ALEXT_PROTOTYPES -DAL_BUILD_LIBRARY -D_GNU_SOURCE=1 -D_LARGEFILE_SOURCE -D_LARGE_FILES -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -std=c99
LOCAL_SRC_FILES := \
    common/atomic.c \
    common/rwlock.c \
    common/threads.c \
    common/uintmap.c \
    OpenAL32/alAuxEffectSlot.c \
    OpenAL32/alBuffer.c \
    OpenAL32/alEffect.c \
    OpenAL32/alError.c \
    OpenAL32/alExtension.c \
    OpenAL32/alFilter.c \
    OpenAL32/alFontsound.c \
    OpenAL32/alListener.c \
    OpenAL32/alMidi.c \
    OpenAL32/alPreset.c \
    OpenAL32/alSoundfont.c \
    OpenAL32/alSource.c \
    OpenAL32/alState.c \
    OpenAL32/alThunk.c \
    OpenAL32/sample_cvt.c \
    Alc/ALc.c \
    Alc/ALu.c \
    Alc/alcConfig.c \
    Alc/alcRing.c \
    Alc/bs2b.c \
    Alc/effects/autowah.c \
    Alc/effects/chorus.c \
    Alc/effects/compressor.c \
    Alc/effects/dedicated.c \
    Alc/effects/distortion.c \
    Alc/effects/echo.c \
    Alc/effects/equalizer.c \
    Alc/effects/flanger.c \
    Alc/effects/modulator.c \
    Alc/effects/null.c \
    Alc/effects/reverb.c \
    Alc/helpers.c \
    Alc/hrtf.c \
    Alc/panning.c \
    Alc/mixer.c \
    Alc/mixer_c.c \
    Alc/midi/base.c \
    Alc/midi/sf2load.c \
    Alc/midi/dummy.c \
    Alc/midi/fluidsynth.c \
    Alc/midi/soft.c \
    Alc/backends/base.c \
    Alc/backends/loopback.c \
    Alc/backends/null.c \
    Alc/backends/opensl.c

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))
    LOCAL_ARM_NEON := true
    LOCAL_SRC_FILES += Alc/mixer_neon.c
endif

include $(BUILD_SHARED_LIBRARY)
