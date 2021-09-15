cmake_minimum_required(VERSION 3.10.2)

project("androidhello")

set(LOCAL_SHARE_PATH ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})

include_directories(include)

add_library(
        native-lib

        SHARED

        native-lib.cpp)

find_library(
        log-lib

        log)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libavcodec.so)

add_library(avdevice SHARED IMPORTED)
set_target_properties(avdevice PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libavdevice.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libavfilter.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libswscale.so)

# if have a static lib
#add_library( yuv STATIC IMPORTED )
#set_target_properties( yuv PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/static/libyuv.a )

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