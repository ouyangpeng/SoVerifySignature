# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
# 设置CMAKE的最小版本
cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#so库的设置
add_library( # Sets the name of the library.
             # so库的名字
             native-lib

             # Sets the library as a shared library.
             #设置库类型为 shared
             SHARED

             # Provides a relative path to your source file(s).
             # 源码路径
             src/main/cpp/native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# 引用库
find_library( # Sets the name of the path variable.
              # 设置库的名称
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              # 设置你要找的NDK库的名称
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# 让你的库可以使用找到的库，需要配置下面这段
target_link_libraries( # Specifies the target library.
                       # 要关联的库
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       # find_library中设置的库的名称
                       ${log-lib} )