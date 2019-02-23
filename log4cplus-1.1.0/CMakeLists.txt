set(SOURCES
    appenderattachableimpl.cxx
    global-init.cxx
    mdc.cxx
    socket.cxx
    appender.cxx
    hierarchy.cxx
    ndc.cxx
    asyncappender.cxx
    hierarchylocker.cxx
    nullappender.cxx
    stringhelper-clocale.cxx
    layout.cxx
    objectregistry.cxx
    stringhelper.cxx
    clogger.cxx
    lockfile.cxx
    patternlayout.cxx
    stringhelper-cxxlocale.cxx
    configurator.cxx
    log4judpappender.cxx
    pointer.cxx
    stringhelper-iconv.cxx
    consoleappender.cxx
    logger.cxx
    property.cxx
    syncprims.cxx
    loggerimpl.cxx
    queue.cxx
    syslogappender.cxx
    env.cxx
    loggingevent.cxx
    rootlogger.cxx
    threads.cxx
    factory.cxx
    loggingmacros.cxx
    sleep.cxx
    timehelper.cxx
    fileappender.cxx
    loglevel.cxx
    snprintf.cxx
    tls.cxx
    fileinfo.cxx
    loglog.cxx
    socketappender.cxx
    version.cxx
    filter.cxx
    logloguser.cxx
    socketbuffer.cxx
)

if (WIN32)
    set(SOURCES ${SOURCES}
        socket-win32.cxx
        win32debugappender.cxx
        nteventlogappender.cxx
        win32consoleappender.cxx
    )
else ()
    set(SOURCES ${SOURCES}
        socket-unix.cxx
    )
endif ()

add_definitions(-DINSIDE_LOG4CPLUS)

add_library(log4cplus STATIC ${SOURCES})

target_link_libraries(log4cplus ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
    target_link_libraries(log4cplus ws2_32)
endif ()
