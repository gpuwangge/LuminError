project(lecommon)
add_library(${PROJECT_NAME} INTERFACE)
#target_compile_definitions(${PROJECT_NAME} INTERFACE EXAMPLE_NAME="BasicTriangles")
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
