project(game_engine)

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
    #${CMAKE_SOURCE_DIR}/external #for yaml headers
    ${CMAKE_SOURCE_DIR}/external/glm/0.9.9.8
    ${CMAKE_SOURCE_DIR}/external/vulkan/1.4.328.1/Include
    ${CMAKE_SOURCE_DIR}/external/sdl/Include
) 

target_link_directories(${PROJECT_NAME} PRIVATE
    #$ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    ${CMAKE_SOURCE_DIR}/external/vulkan/1.4.328.1/Lib
    ${CMAKE_SOURCE_DIR}/external/sdl #SDL3.dll, SDL3_ttf.dll
) 
target_link_libraries(${PROJECT_NAME} PRIVATE leinterface vulkan-1 SDL3 SDL3_ttf lecommon) #textmanager need sdl2 and sdl3_ttf

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.dll <<<<<<<<<<<<<<<<<<"
)
