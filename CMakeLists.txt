cmake_minimum_required(VERSION 3.20)
SET(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
SET(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")
project(VulkanPlatform)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

add_definitions(-DSDL)

include_directories(
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}
    ${PROJECT_SOURCE_DIR}/vulkanFramework/include
    ${PROJECT_SOURCE_DIR}/thirdparty)

link_directories(
    $ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    $ENV{LIB}
    ${PROJECT_SOURCE_DIR}/thirdparty #put SDL3.dll, SDL3_ttf.dll and glfw3.dll in thirdparty/
    ${PROJECT_SOURCE_DIR}/thirdparty/yaml-cpp/lib) 

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #this is required in Windows, all dll/exe goes here
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #this is not required in windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #all lib goes here

add_subdirectory(common)
add_subdirectory(interface)
add_subdirectory(sdlcore)
add_subdirectory(application)
add_subdirectory(example)
add_subdirectory(launcher)

file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/sdl3.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/sdl3_ttf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/fonts/NotoSansCJK-VF.otf.ttc DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/yaml-cpp/lib/libyaml-cpp.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

# 创建 autotest.bat 文件
file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat "@echo off\r\n")
file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat "echo Starting Auto Tests...\r\n\r\n")
# 定义示例目录
set(EXAMPLE_DIRS BasicTriangles Bptpc16Texture SimpleTriangle)
foreach(example_dir IN LISTS EXAMPLE_DIRS)
    # 获取目录中的 CPP 文件
    file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/example/${example_dir}/*.cpp)
    foreach(sample_file IN LISTS SRC_FILES)
        get_filename_component(sample_name ${sample_file} NAME_WE)
        # 添加到批处理文件（Windows 需要 .exe 扩展名）
        file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat 
            "start /wait launcher.exe ${sample_name}.dll\r\n"
        )
    endforeach()
endforeach()
file(APPEND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autotest.bat "\r\necho All tests completed!\r\n")

message(STATUS "=====Print Variables=====")
message(STATUS "ENV{LIB} = $ENV{LIB}")
message(STATUS "ENV{INCLUDE} = $ENV{INCLUDE}")
message(STATUS "ENV{VULKAN_SDK} = $ENV{VULKAN_SDK}")
#message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}") #same as PROJECT_SOURCE_DIR
message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}") #LuminError
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}") #build
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "=========================")