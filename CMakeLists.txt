cmake_minimum_required(VERSION 3.20)
SET(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
SET(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")
project(VulkanPlatform)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

#Set variables so all cmakelists.txt in sub dirs can access
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #this is required in Windows, all dll/exe goes here
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #this is not required in windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #all lib goes here

add_subdirectory(common)
add_subdirectory(interface)
add_subdirectory(logcore)
add_subdirectory(yamlcore)
add_subdirectory(sdlcore)
add_subdirectory(resourcecore)
add_subdirectory(core_renderer)
add_subdirectory(gameengine)
add_subdirectory(example)

file(COPY ${PROJECT_SOURCE_DIR}/external/sdl/sdl3.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/external/sdl/sdl3_ttf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/external/fonts/NotoSansCJK-VF.otf.ttc DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/external/yaml-cpp/lib/libyaml-cpp.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

# 创建 autotest.bat 文件
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat "@echo off\r\n")
file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat "echo Starting Auto Tests...\r\n\r\n")
# 自动发现所有示例目录
# file(GLOB EXAMPLE_DIRS RELATIVE ${PROJECT_SOURCE_DIR}/example "*")
# 自动发现 example 目录下的所有子目录
file(GLOB EXAMPLE_DIRS RELATIVE ${PROJECT_SOURCE_DIR}/example ${PROJECT_SOURCE_DIR}/example/*)
# 过滤出目录（排除文件）
set(EXAMPLE_DIRECTORIES "")
foreach(dir IN LISTS EXAMPLE_DIRS)
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/example/${dir})
        list(APPEND EXAMPLE_DIRECTORIES ${dir})
    endif()
endforeach()
#message(STATUS "Root CMakeLists.txt: Found example directories: ${EXAMPLE_DIRECTORIES}")
foreach(example_dir IN LISTS EXAMPLE_DIRECTORIES)
    # 获取目录中的 CPP 文件
    file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/example/${example_dir}/*.cpp)
    foreach(sample_file IN LISTS SRC_FILES)
        get_filename_component(sample_name ${sample_file} NAME_WE)
        # 添加到批处理文件（Windows 需要 .exe 扩展名）
        file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat 
            "start /wait ${sample_name}.exe\r\n"
        )
    endforeach()
endforeach()
file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat "\r\necho All tests completed!\r\n")

# message(STATUS "=====Print Variables=====")
# message(STATUS "ENV{LIB} = $ENV{LIB}")
# message(STATUS "ENV{INCLUDE} = $ENV{INCLUDE}")
# message(STATUS "ENV{VULKAN_SDK} = $ENV{VULKAN_SDK}")
# message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}") #LuminError
# message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}") #LuminError
# message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}") #LuminError/build
# message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}") #LuminError/build/bin
# message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}") #LuminError/build/lib
# message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}") #LuminError/build/lib
# message(STATUS "=========================")

#message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}") #same as PROJECT_SOURCE_DIR