set(SOURCES
    common/shapes.cc
    sweep/advancing_front.cc
    sweep/cdt.cc
    sweep/sweep.cc
    sweep/sweep_context.cc
)

include_directories(${AIRFORCE_INCLUDE_DIR}/common)
include_directories(${AIRFORCE_INCLUDE_DIR}/sweep)

add_library(poly2tri STATIC ${SOURCES})

target_link_libraries(poly2tri ${CMAKE_THREAD_LIBS_INIT})
