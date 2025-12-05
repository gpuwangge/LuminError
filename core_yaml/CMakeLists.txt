project(core_yaml)

file(GLOB SOURCES "source/*.cpp")
file(GLOB HEADERS "include/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    PREFIX ""
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    #$ENV{VULKAN_SDK}/Include
    ${CMAKE_SOURCE_DIR}/external/vulkan/1.4.328.1/Include
    ${CMAKE_SOURCE_DIR}/external #for yaml-cpp/ headers
)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/yaml-cpp/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE leinterface yaml-cpp lecommon)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.dll <<<<<<<<<<<<<<<<<<"
)

# message(STATUS "=====Print Variables=====")
# message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}") #LuminError
# message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}") #LuminError/yamlcore
# message(STATUS "=========================")