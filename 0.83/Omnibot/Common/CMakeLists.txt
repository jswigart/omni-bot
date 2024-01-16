# Omnibot common build


file(GLOB COMMON_SRC CONFIGURE_DEPENDS
    ${DEPS_PATH}/gmscriptex/gmsrc_ex/src/gm/*.cpp
    ${DEPS_PATH}/gmscriptex/gmsrc_ex/src/platform/win32gcc/*.cpp
    ${DEPS_PATH}/gmscriptex/gmsrc_ex/src/binds/*.cpp
    ${DEPS_PATH}/gmscriptex/gmsrc_ex/src/binds/gmSqliteLib.cpp
    ${DEPS_PATH}/gmscriptex/gmsrc_ex/src/3rdParty/mathlib/*.cpp
    ${DEPS_PATH}/gmscriptex/gmsrc_ex/src/3rdParty/gmbinder2/*.cpp
    ${DEPS_PATH}/wildmagic/*.cpp
    ${DEPS_PATH}/physfs/*.c
    ${DEPS_PATH}/physfs/archivers/*.c
    ${DEPS_PATH}/physfs/archivers/*.c
    ${DEPS_PATH}/physfs/lzma/C/*.c
    ${DEPS_PATH}/physfs/lzma/C/Archive/7z/*.c
    ${DEPS_PATH}/physfs/lzma/C/Compress/Lzma/*.c
    ${DEPS_PATH}/physfs/lzma/C/Compress/Branch/*.c
    ${DEPS_PATH}/physfs/zlib123/*.c
    ${DEPS_PATH}/physfs/platform/*.c

    ${DEPS_PATH}/Recast/Recast/Source/*.cpp
    ${DEPS_PATH}/Recast/Detour/Source/*.cpp
    # ${DEPS_PATH}/Recast/DetourTileCache/Source/*.cpp
    ${DEPS_PATH}/Recast/DebugUtils/Source/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/ObstacleManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RecastInterfaces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BatchBuild.cpp
)

list(FILTER COMMON_SRC EXCLUDE REGEX ".*gmSqliteLib\\.cpp$")

add_library(omnibot-common STATIC ${COMMON_SRC})
set_property(TARGET omnibot-common PROPERTY CXX_STANDARD 11)
set_property(TARGET omnibot-common PROPERTY C_STANDARD 99)

target_include_directories(omnibot-common
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"

    PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/3rdParty"
    # PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/3rdParty/mathlib"
    PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/gm"
    PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/binds"
    PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/3rdParty/gmbinder2"

    PUBLIC "${DEPS_PATH}/wildmagic"
    PUBLIC "${DEPS_PATH}/iprof"

    PUBLIC "${DEPS_PATH}/Recast/Common/include"
    PUBLIC "${DEPS_PATH}/Recast/Recast/Include"
    PUBLIC "${DEPS_PATH}/Recast/DebugUtils/Include"
    PUBLIC "${DEPS_PATH}/Recast/Detour/Include"
    PUBLIC "${DEPS_PATH}/Recast/DetourTileCache/Include"
    PUBLIC "${DEPS_PATH}/physfs"
    PUBLIC "${DEPS_PATH}/physfs/lzma/C"
    PUBLIC "${DEPS_PATH}/physfs/zlib123"
    PUBLIC "${DEPS_PATH}/physfs/lzma/C/Archive/7z"
    PUBLIC "${DEPS_PATH}/physfs/lzma/C/Compress/Lzma"
    PUBLIC "${DEPS_PATH}/physfs/lzma/C/Compress/Branch"
    # PUBLIC "${DEPS_PATH}/Remotery/lib"
)

if (WIN32)
    if (MSVC)
        target_include_directories(omnibot-common PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/platform/win32msvc")
    else()
        target_include_directories(omnibot-common PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/platform/win32gcc")
    endif()
elseif(APPLE)
    target_include_directories(omnibot-common PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/platform/osx")
elseif(LINUX)
    target_include_directories(omnibot-common PUBLIC "${DEPS_PATH}/gmscriptex/gmsrc_ex/src/platform/win32gcc")
endif()

target_link_libraries(omnibot-common PUBLIC omni-shared)
target_compile_definitions(omnibot-common PUBLIC PHYSFS_SUPPORTS_ZIP=1 PHYSFS_SUPPORTS_7Z=1 PHYSFS_NO_CDROM_SUPPORT)
target_compile_definitions(omnibot-common PUBLIC INTERPROCESS=1)
set_target_properties(omnibot-common PROPERTIES COMPILE_FLAGS -pthread -ffriend-injection -fno-strict-aliasing)

if(APPLE)
    find_library(CARBON_FW Carbon)
    target_link_libraries(omnibot-common PUBLIC ${CARBON_FW})
    find_library(IOKIT_FW IOKit)
    target_link_libraries(omnibot-common PUBLIC ${IOKIT_FW})
endif()
