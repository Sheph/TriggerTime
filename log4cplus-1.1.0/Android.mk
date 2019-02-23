LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblog4cplus
LOCAL_CFLAGS := -DINSIDE_LOG4CPLUS
LOCAL_SRC_FILES := \
    appenderattachableimpl.cxx \
    global-init.cxx \
    mdc.cxx \
    socket.cxx \
    appender.cxx \
    hierarchy.cxx \
    ndc.cxx \
    socket-unix.cxx \
    asyncappender.cxx \
    hierarchylocker.cxx \
    nullappender.cxx \
    stringhelper-clocale.cxx \
    layout.cxx \
    objectregistry.cxx \
    stringhelper.cxx \
    clogger.cxx \
    lockfile.cxx \
    patternlayout.cxx \
    stringhelper-cxxlocale.cxx \
    configurator.cxx \
    log4judpappender.cxx \
    pointer.cxx \
    stringhelper-iconv.cxx \
    consoleappender.cxx \
    logger.cxx \
    property.cxx \
    syncprims.cxx \
    loggerimpl.cxx \
    queue.cxx \
    syslogappender.cxx \
    env.cxx \
    loggingevent.cxx \
    rootlogger.cxx \
    threads.cxx \
    factory.cxx \
    loggingmacros.cxx \
    sleep.cxx \
    timehelper.cxx \
    fileappender.cxx \
    loglevel.cxx \
    snprintf.cxx \
    tls.cxx \
    fileinfo.cxx \
    loglog.cxx \
    socketappender.cxx \
    version.cxx \
    filter.cxx \
    logloguser.cxx \
    socketbuffer.cxx

include $(BUILD_STATIC_LIBRARY)
