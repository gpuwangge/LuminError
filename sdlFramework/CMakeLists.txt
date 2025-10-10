project(sdl-vulkan-framework)
aux_source_directory(${PROJECT_SOURCE_DIR}/source SRC)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build)
#add_library(${PROJECT_NAME} STATIC ${SRC})


#find_package(SDL3 REQUIRED)
#include_directories(${SDL3_INCLUDE_DIRS})
#include_directories($ENV{VULKAN_SDK}/Include)
#link_directories(
#    $ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
#    $ENV{LIB}) 

include_directories(
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}
    ${PROJECT_SOURCE_DIR}/vulkanFramework/include
    ${PROJECT_SOURCE_DIR}/thirdparty)

link_directories(
    $ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    $ENV{LIB}
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty #put SDL3.dll, SDL3_ttf.dll and glfw3.dll in thirdparty/
    ${PROJECT_SOURCE_DIR}/thirdparty/yaml-cpp/lib) 

#set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin) #this is not required in windows
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin) #this is required in Windows, all dll/exe goes here
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(${PROJECT_NAME} SHARED ${SRC})
#target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
#link_libraries(SDL3)
target_link_libraries(${PROJECT_NAME} SDL3)
