cmake_minimum_required(VERSION 2.6)

project(REDPIXEL C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(PkgConfig)
pkg_search_module(ALLEGRO5 REQUIRED allegro-5 allegro-5.0)
pkg_search_module(ALLEGRO5_AUDIO REQUIRED allegro_audio-5 allegro_audio-5.0)
pkg_search_module(ALLEGRO5_ACODEC REQUIRED allegro_acodec-5 allegro_acodec-5.0)
pkg_search_module(ALLEGRO5_FONT REQUIRED allegro_font-5 allegro_font-5.0)
pkg_search_module(ALLEGRO5_IMAGE REQUIRED allegro_image-5 allegro_image-5.0)
pkg_search_module(ALLEGRO5_PRIMITIVES REQUIRED allegro_primitives-5 allegro_primitives-5.0)

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

include_directories(SYSTEM
    ${ALLEGRO5_INCLUDE_DIRS}
    ${ALLEGRO5_AUDIO_INCLUDE_DIRS}
    ${ALLEGRO5_ACODEC_INCLUDE_DIRS}
    ${ALLEGRO5_FONT_INCLUDE_DIRS}
    ${ALLEGRO5_IMAGE_INCLUDE_DIRS}
    ${ALLEGRO5_PRIMITIVES_INCLUDE_DIRS}
    # To work around timer.h conflict in libnet.
    ${ALLEGRO4_to_5_INCLUDE_DIRS}
    )

include_directories(
    src
    ${REDPIXEL_INCLUDE_DIRS}
    ${AGUP_INCLUDE_DIRS}
    ${LIBNET_INCLUDE_DIRS}
    )

link_directories(
    ${ALLEGRO5_LIBDIR}
    ${ALLEGRO5_AUDIO_LIBDIR}
    ${ALLEGRO5_ACODEC_LIBDIR}
    ${ALLEGRO5_FONT_LIBDIR}
    ${ALLEGRO5_IMAGE_LIBDIR}
    ${ALLEGRO5_PRIMITIVES_LIBDIR}
    )

add_executable(redpixel
    ${ALLEGRO4_to_5_SOURCES}
    ${REDPIXEL_SOURCES}
    ${AGUP_SOURCES}
    ${LIBNET_SOURCES}
    )

target_link_libraries(redpixel
    ${ALLEGRO5_LIBRARIES}
    ${ALLEGRO5_AUDIO_LIBRARIES}
    ${ALLEGRO5_ACODEC_LIBRARIES}
    ${ALLEGRO5_FONT_LIBRARIES}
    ${ALLEGRO5_IMAGE_LIBRARIES}
    ${ALLEGRO5_PRIMITIVES_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -lm
    )

# vim: set sts=4 sw=4 et:
