cmake_minimum_required(VERSION 3.10.2)

project("androidhello")

set(LOCAL_SHARE_PATH ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})  #可以不写
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")

include_directories(src/main/cpp/include)

add_library(
        native-lib

        SHARED

        src/main/cpp/native-lib.cpp)

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