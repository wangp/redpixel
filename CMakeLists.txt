cmake_minimum_required(VERSION 2.6)

project(REDPIXEL C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(PkgConfig)
pkg_check_modules(ALLEGRO REQUIRED allegro)

# Avoid link errors by linking to pthread directly.
find_package(Threads)

include(FileList)

if(UNIX)
    add_definitions(-DTARGET_LINUX)
endif()
if(WIN32)
    add_definitions(-DTARGET_WINDOWS)
endif()
if(MINGW)
    add_definitions(-DTARGET_MINGW32)
endif()

# Too many unused parameters in old code for now.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wno-unused")

add_executable(redpixel
    ${REDPIXEL_SOURCES}
    ${AGUP_SOURCES}
    ${LIBNET_SOURCES}
    ${JGMOD_SOURCES}
    ${LIBCDA_SOURCES}
    )

include_directories(SYSTEM
    ${ALLEGRO_INCLUDE_DIRS}
    )

include_directories(
    ${REDPIXEL_INCLUDE_DIRS}
    ${AGUP_INCLUDE_DIRS}
    ${LIBNET_INCLUDE_DIRS}
    ${JGMOD_INCLUDE_DIRS}
    ${LIBCDA_INCLUDE_DIRS}
    )

target_link_libraries(redpixel
    ${ALLEGRO_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -lm
    )

# vim: set sts=4 sw=4 et:
