cmake_minimum_required(VERSION 3.10.2)

project("ffmpegplayer")

set(LOCAL_SHARE_PATH ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI}")

include_directories(include)

add_library(
        native-lib

        SHARED

        native-lib.cpp)

find_library(
        log-lib

        log)

target_link_libraries(
        native-lib
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        ${log-lib})