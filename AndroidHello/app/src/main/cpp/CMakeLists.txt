cmake_minimum_required(VERSION 3.10.2)

project("androidhello")

set(LOCAL_SHARE_PATH ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})  #可以不写
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI}")

include_directories(include)

add_library(
        native-lib

        SHARED

        native-lib.cpp)

find_library(
        log-lib

        log)

#第一种形式：
#target_link_libraries(
#        native-lib
#        avcodec
#        avdevice
#        avfilter
#        avformat
#        avutil
#        swresample
#        swscale
#        ${log-lib})

#第二种形式：
set(FFmpeglib
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        )
target_link_libraries(
        native-lib
        ${FFmpeglib}
        ${log-lib})