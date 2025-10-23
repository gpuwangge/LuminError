project(application)

include_directories(
    include
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}
    ${CMAKE_SOURCE_DIR}/application/include
    ${CMAKE_SOURCE_DIR}/thirdparty)

link_directories(
    $ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    $ENV{LIB}
    ${CMAKE_SOURCE_DIR}/thirdparty/sdl #put SDL3.dll, SDL3_ttf.dll and glfw3.dll in thirdparty/
    ${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp/lib) 

file(GLOB SOURCES "source/*.cpp")
file(GLOB HEADERS "include/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
   OUTPUT_NAME ${PROJECT_NAME}
   PREFIX ""
)

#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE leinterface vulkan-1 SDL3 SDL3_ttf lecommon) #sdl-vulkan-framework 

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.dll <<<<<<<<<<<<<<<<<<"
)
