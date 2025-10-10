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


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) #this is required in Windows, all dll/exe goes here
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) #this is not required in windows
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build) #all lib goes here

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)




add_subdirectory(interface)

add_subdirectory(sdlframework)
add_subdirectory(vulkanFramework)
#link_libraries(vulkan-framework sdl-vulkan-framework vulkan-1 SDL3 SDL3_ttf yaml-cpp)

# set(SAMPLE_NAME simpleTriangle)
# add_executable(${SAMPLE_NAME} samples/${SAMPLE_NAME}.cpp)

# add_executable(mainbinary vulkanFramework/include/main.cpp) use launcher to replace this

#add_subdirectory(application)
#add_subdirectory(sdl_core)
#add_subdirectory(example)
add_subdirectory(launcher)


file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/sdl3.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/sdl3_ttf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/fonts/NotoSansCJK-VF.otf.ttc DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
file(COPY ${PROJECT_SOURCE_DIR}/thirdparty/yaml-cpp/lib/libyaml-cpp.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)



message(STATUS "=====Print Variables=====")
message(STATUS "ENV{LIB} = $ENV{LIB}")
message(STATUS "ENV{INCLUDE} = $ENV{INCLUDE}")
message(STATUS "ENV{VULKAN_SDK} = $ENV{VULKAN_SDK}")
#message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}") #same as PROJECT_SOURCE_DIR
message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "=========================")