project(core_resource)

file(GLOB SOURCES "source/*.cpp")
file(GLOB HEADERS "include/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    PREFIX ""
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    #$ENV{INCLUDE} #for glm
    #$ENV{VULKAN_SDK}/Include
    #${CMAKE_SOURCE_DIR}/external #for yaml headers
    ${CMAKE_SOURCE_DIR}/external/glm/0.9.9.8
    ${CMAKE_SOURCE_DIR}/external/vulkan/1.4.328.1/Include
    ${CMAKE_SOURCE_DIR}/external/sdl/Include
    ${CMAKE_SOURCE_DIR}/external/tinygltf/Include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    #$ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    ${CMAKE_SOURCE_DIR}/external/vulkan/1.4.328.1/Lib
) 
#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/yaml-cpp/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE leinterface vulkan-1 lecommon)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.dll <<<<<<<<<<<<<<<<<<"
)
