cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0014 NEW)

set(CMAKE_CXX_STANDARD 98)

option(CROSSCOMPILING "Enable cross-compiling")

if (CROSSCOMPILING)
    if (NOT CMAKE_FIND_ROOT_PATH_MODE_PROGRAM)
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    endif ()
    if (NOT CMAKE_FIND_ROOT_PATH_MODE_LIBRARY)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    endif ()
    if (NOT CMAKE_FIND_ROOT_PATH_MODE_INCLUDE)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    endif ()
endif ()

project(AIRFORCE)
enable_language(ASM)
include(CheckTypeSize)

#platform setup

if (WIN32)
    set(PLATFORM_NAME "win32")
else ()
    CHECK_TYPE_SIZE("void*" PTR_SIZE BUILTIN_TYPES_ONLY)
    if (PTR_SIZE EQUAL 8)
        set(PLATFORM_NAME "linux64")
    else ()
        set(PLATFORM_NAME "linux32")
    endif ()
endif ()

# USER SETTINGS

if (NOT WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif ()
endif ()

option(BUILD_DOC "Build lua API documentation" FALSE)
option(USE_LUAJIT "Use LuaJIT" FALSE)
option(USE_LUAJIT_VALGRIND "Enable LuaJIT+valgrind" FALSE)
option(USE_STEAM_API "Enable Steam API" TRUE)
option(INSTALL_BETA "Install beta version" FALSE)

# END USER SETTINGS

set(INSTALL_OUT_DIR "out" CACHE STRING "Where to install output files")

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)

#out dir

set(AIRFORCE_OUT_DIR ${AIRFORCE_BINARY_DIR}/out)

#set includes/libs

set(AIRFORCE_INCLUDE_DIR "${AIRFORCE_SOURCE_DIR}/include")
include_directories(${AIRFORCE_INCLUDE_DIR})

#find packages

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_INCLUDEDIR "${AIRFORCE_INCLUDE_DIR}")
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Threads)
if (NOT WIN32)
    find_package(X11 REQUIRED)
endif ()
set(ZLIB_INCLUDE_DIRS "${AIRFORCE_SOURCE_DIR}/zlib-1.2.8")
set(ZLIB_LIBRARIES "z")

if (USE_STEAM_API)
    add_definitions(-DUSE_STEAM_API)
    find_library(STEAM_API_LIBRARIES NAMES steam_api HINTS "${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
endif ()

#find programs

find_program(LDOC_EXECUTABLE NAMES ldoc)

add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_CHRONO_HEADER_ONLY -DBOOST_NO_CXX11_VARIADIC_TEMPLATES)

if (NOT USE_LUAJIT)
    add_definitions(-DLUABIND_CPLUSPLUS_LUA)
endif ()

if (USE_LUAJIT_VALGRIND)
    add_definitions(-DLUAJIT_USE_VALGRIND -DLUAJIT_USE_SYSMALLOC)
    if (NOT BUILD_TYPE_LOWER STREQUAL "debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif ()
endif ()

if (NOT WIN32)
    if (BUILD_TYPE_LOWER STREQUAL "debug")
        if (USE_LUAJIT)
            add_definitions(-DLUA_USE_APICHECK -DLUA_USE_ASSERT)
        else ()
            add_definitions(-DLUA_USE_APICHECK -Dlua_assert=assert)
        endif ()
    endif ()
else ()
    if (USE_LUAJIT)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DLUA_USE_APICHECK /DLUA_USE_ASSERT /MTd /bigobj")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DLUA_USE_APICHECK /DLUA_USE_ASSERT /MTd /bigobj")
    else ()
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DLUA_USE_APICHECK /Dlua_assert=assert /MTd /bigobj")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DLUA_USE_APICHECK /Dlua_assert=assert /MTd /bigobj")
    endif ()
    foreach (CONFIGURATION RELEASE MINSIZEREL RELWITHDEBINFO)
        set(CMAKE_C_FLAGS_${CONFIGURATION} "${CMAKE_C_FLAGS_${CONFIGURATION}} /MT /bigobj")
        set(CMAKE_CXX_FLAGS_${CONFIGURATION} "${CMAKE_CXX_FLAGS_${CONFIGURATION}} /MT /bigobj")
    endforeach ()
endif ()

if (NOT WIN32)
    include_directories(${X11_INCLUDE_DIR})
endif ()
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${AIRFORCE_SOURCE_DIR}/libRocket/Include)

if (USE_LUAJIT)
    include_directories(${AIRFORCE_INCLUDE_DIR}/luajit)
else ()
    include_directories(${AIRFORCE_INCLUDE_DIR}/lua)
endif ()

#fix output directories

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AIRFORCE_OUT_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AIRFORCE_OUT_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${AIRFORCE_OUT_DIR}/lib)

if (WIN32)
    add_definitions(-DBYPASS_ECLIPSE_INDEXER -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -D_REENTRANT -fPIC -DPIC -DBYPASS_ECLIPSE_INDEXER")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -D_REENTRANT -fPIC -DPIC -DBYPASS_ECLIPSE_INDEXER")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
endif ()
add_definitions(-DROCKET_STATIC_LIB)

message(STATUS "Platform - " ${CMAKE_SYSTEM_NAME})
message(STATUS "Install prefix - " ${CMAKE_INSTALL_PREFIX})
if (BUILD_DOC)
    if (LDOC_EXECUTABLE)
        message(STATUS "ldoc found, lua API doc will be generated")
    else ()
        message(STATUS "ldoc NOT found, lua API doc will NOT be generated")
    endif ()
endif ()

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined -Wl,--exclude-libs,ALL")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -Wl,--exclude-libs,ALL")
endif ()

add_subdirectory(zlib-1.2.8)
add_subdirectory(boost_1_55_0)
add_subdirectory(Box2D_v2.3.0)
if (USE_LUAJIT)
    add_subdirectory(LuaJIT-2.0.4)
else ()
    add_subdirectory(lua-5.1.5)
endif ()
add_subdirectory(luabind-0.9.1)
add_subdirectory(libpng-1.6.8)
add_subdirectory(log4cplus-1.1.0)
add_subdirectory(jsoncpp)
add_subdirectory(poly2tri)
add_subdirectory(ogg-1.3.2)
add_subdirectory(vorbis-1.3.5)
add_subdirectory(freetype-2.6.2)
add_subdirectory(libRocket)
add_subdirectory(afutil)
add_subdirectory(game)
if (BUILD_DOC AND LDOC_EXECUTABLE)
    add_subdirectory(apidoc)
endif ()
