
cmake_minimum_required(VERSION 3.10)

project(Omnibot)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

macro(add_cxx_flag flag)
    message(STATUS "Configure with flag '${flag}'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
endmacro()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(DEPS_PATH "${CMAKE_SOURCE_DIR}/dependencies")

add_library(omni-shared INTERFACE)

set(Boost_USE_STATIC_LIBS       OFF)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost COMPONENTS system filesystem regex date_time REQUIRED)
target_link_libraries(omni-shared INTERFACE Boost::boost Boost::filesystem Boost::regex Boost::date_time)

target_include_directories(omni-shared INTERFACE "${DEPS_PATH}/WildMagic5/SDK/include")
# target_include_directories(omni-shared "dependencies/wildmagic")

# target_include_directories(omni-shared "dependencies/gmsrc_ex/")

add_compile_options(-Wall -Wno-deprecated -fno-strict-aliasing)
add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")

add_subdirectory(Common)
add_subdirectory(ET)
