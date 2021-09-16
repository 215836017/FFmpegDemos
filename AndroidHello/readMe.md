1. app
   1. CMakeLists.txt在cpp文件夹内
   2. 使用 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI}") 语句链接库
   3. build.gradle(app)中需要添加jniLibs语句
           sourceSets {
               main {
                   jniLibs.srcDirs = ['libs']
               }
           }

2. app2  -- 和app进行对比
   1. CMakeLists.txt在cpp文件夹内
   2. 使用如下语句进行链接库
      set(LOCAL_SHARE_PATH ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})
      
      add_library(avcodec SHARED IMPORTED)
      set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${LOCAL_SHARE_PATH}/libavcodec.so)
   3. build.gradle(app)中不 不 不需要添加jniLibs语句，否则编译不通过
   
3. app3  --  和app进行对比
   1. CMakeLists.txt在cpp文件夹外部，在app3内，所以在build.gradle中申明CMakeLists.txt的路径有变化：
      externalNativeBuild {
              cmake {
                  path file('CMakeLists.txt')
                  version '3.10.2'
              }
          }
   2. CMakeLists.txt引用库和代码文件的路径也会变化，比如：
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")
      include_directories(src/main/cpp/include)
   
4. app4  -- 对比app
   1. app4中使用的是静态库，但是库的路径不变
   2. build.gradle中要指明是静态库：  
      arguments  '-DANDROID_STL=c++_static'
   3. 其他写法和app中一致