
cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jnilibs ${CMAKE_SOURCE_DIR}/../../../libs)

project("androidhello")

include_directories(
        include
)

#第一种链接写法
#link_directories(
#        ${jnilibs}/${ANDROID_ABI})

#第二种链接写法
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI}")

file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        )

add_library(
        native-lib

        SHARED

        native-lib.cpp)

set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        fdk-aac
        x264
        )

set(native-libs
        android
        EGL
        GLESv3
        OpenSLES
        log
        m
        z
        )

target_link_libraries( # Specifies the target library.
        native-lib

        ${log-lib}
        #        ${third-party-libs}
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        fdk-aac
        x264
        ${native-libs}
        )