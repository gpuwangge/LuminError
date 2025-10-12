project(application)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES "source/*.cpp")
file(GLOB HEADERS "include/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
   OUTPUT_NAME ${PROJECT_NAME}
   PREFIX ""
)

#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE leinterface vulkan-1 SDL3 SDL3_ttf yaml-cpp lecommon) #sdl-vulkan-framework 

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${PROJECT_NAME}.dll <<<<<<<<<<<<<<<<<<"
)
