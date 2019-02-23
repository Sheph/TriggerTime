set(SOURCES
    error_code.cpp
    future.cpp
)

if (NOT WIN32)
    set(SOURCES ${SOURCES}
        pthread/thread.cpp
        pthread/once.cpp
    )
else ()
    set(SOURCES ${SOURCES}
        win32/thread.cpp
        win32/tss_dll.cpp
        win32/tss_pe.cpp
    )
endif ()

add_definitions(-DBOOST_SYSTEM_STATIC_LINK=1 -DBOOST_THREAD_BUILD_LIB=1)

if (NOT WIN32)
    add_definitions(-DBOOST_THREAD_POSIX)
endif ()

add_library(boost STATIC ${SOURCES})

target_link_libraries(boost ${CMAKE_THREAD_LIBS_INIT})
