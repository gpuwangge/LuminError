project(yamlcore)
 
include_directories(
    include
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}
    ${CMAKE_SOURCE_DIR}/application/include
    ${CMAKE_SOURCE_DIR}/thirdparty)

link_directories(
    $ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    $ENV{LIB}
    ${CMAKE_SOURCE_DIR}/thirdparty #put SDL3.dll, SDL3_ttf.dll and glfw3.dll in thirdparty/
    ${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp/lib) 

file(GLOB SOURCES "source/*.cpp")
file(GLOB HEADERS "include/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    PREFIX ""
)

target_link_libraries(${PROJECT_NAME} leinterface yaml-cpp lecommon)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.dll <<<<<<<<<<<<<<<<<<"
)

# message(STATUS "=====Print Variables=====")
# message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}") #LuminError
# message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}") #LuminError/yamlcore
# message(STATUS "=========================")