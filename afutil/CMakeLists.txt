set(SOURCES
    Logger.cpp
    PNGDecoder.cpp
    PNGUtils.cpp
    Utils.cpp
    EnumTraits.cpp
    RUBEBase.cpp
    RUBEBody.cpp
    RUBEFixture.cpp
    RUBEImage.cpp
    RUBEJoint.cpp
    RUBEWorld.cpp
    RUBEObject.cpp
    TPS.cpp
    GdxP.cpp
    StreamAppConfig.cpp
    SequentialAppConfig.cpp
    AudioDevice.cpp
    AudioOggFile.cpp
    AudioSound.cpp
    AudioSourceImpl.cpp
    AudioStream.cpp
    AudioListener.cpp
    OAL.cpp
)

add_library(afutil STATIC ${SOURCES})

target_link_libraries(afutil jsoncpp log4cplus Box2D png boost vorbisfile)
