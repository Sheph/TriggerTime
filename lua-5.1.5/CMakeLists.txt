set(SOURCES
    lapi.cpp
    lcode.cpp
    ldo.cpp
    lgc.cpp
    llex.cpp
    loadlib.cpp
    loslib.cpp
    lstring.cpp
    ltablib.cpp
    lzio.cpp
    lauxlib.cpp
    ldblib.cpp
    ldump.cpp
    linit.cpp
    lmathlib.cpp
    lobject.cpp
    lparser.cpp
    lstrlib.cpp
    ltm.cpp
    lundump.cpp
    lbaselib.cpp
    ldebug.cpp
    lfunc.cpp
    liolib.cpp
    lmem.cpp
    lopcodes.cpp
    lstate.cpp
    ltable.cpp
    lvm.cpp
)

if (NOT WIN32)
    add_definitions(-DLUA_USE_LINUX)
endif ()

include_directories(.)

add_library(lua STATIC ${SOURCES})

add_executable(luac luac.cpp print.cpp)

target_link_libraries(luac lua)
