project(glfw-framework)

aux_source_directory(${PROJECT_SOURCE_DIR}/source SRC)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build)
add_library(${PROJECT_NAME} STATIC ${SRC})

