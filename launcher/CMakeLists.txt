project(launcher)

include_directories(
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}
    ${CMAKE_SOURCE_DIR}/vulkanFramework/include
    ${CMAKE_SOURCE_DIR}/thirdparty)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE leinterface)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.exe <<<<<<<<<<<<<<<<<<"
)

