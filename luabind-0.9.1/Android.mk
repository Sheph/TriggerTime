LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libluabind
LOCAL_SRC_FILES := \
    class.cpp \
    create_class.cpp \
    inheritance.cpp \
    pcall.cpp \
    wrapper_base.cpp \
    class_info.cpp \
    error.cpp \
    link_compatibility.cpp \
    scope.cpp \
    class_registry.cpp \
    exception_handler.cpp \
    object_rep.cpp \
    stack_content_by_name.cpp \
    class_rep.cpp \
    function.cpp \
    open.cpp \
    weak_ref.cpp

include $(BUILD_STATIC_LIBRARY)
