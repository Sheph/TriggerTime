APP_ABI := armeabi armeabi-v7a x86
APP_PLATFORM := android-9
APP_STL := gnustl_static
APP_CPPFLAGS += -fexceptions -frtti -fvisibility-inlines-hidden
APP_CFLAGS += -DBOOST_ALL_NO_LIB -DBOOST_CHRONO_HEADER_ONLY -DLUABIND_CPLUSPLUS_LUA -DBYPASS_ECLIPSE_INDEXER -I../include -fvisibility=hidden
ifeq ($(APP_OPTIM),debug)
    APP_CFLAGS += -DLUA_USE_APICHECK -Dlua_assert=assert
else
    APP_CFLAGS += -O3
endif
APP_LDFLAGS += -Wl,--no-undefined -Wl,--exclude-libs,ALL
