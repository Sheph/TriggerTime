set(VORBIS_SOURCES
    mdct.c
    smallft.c
    block.c
    envelope.c
    window.c
    lsp.c
    lpc.c
    analysis.c
    synthesis.c
    psy.c
    info.c
    floor1.c
    floor0.c
    res0.c
    mapping0.c
    registry.c
    codebook.c
    sharedbook.c
    lookup.c
    bitrate.c
)

set(VORBISFILE_SOURCES
    vorbisfile.c
)

set(VORBISENC_SOURCES
    vorbisenc.c
)

include_directories(.)

add_library(vorbis STATIC ${VORBIS_SOURCES})
target_link_libraries(vorbis ogg)

add_library(vorbisfile STATIC ${VORBISFILE_SOURCES})
target_link_libraries(vorbisfile vorbis)

add_library(vorbisenc STATIC ${VORBISENC_SOURCES})
target_link_libraries(vorbisenc vorbis)
