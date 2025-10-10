project(vulkan-framework)
aux_source_directory(${PROJECT_SOURCE_DIR}/source SRC)
add_library(${PROJECT_NAME} STATIC ${SRC})
#add_library(${PROJECT_NAME} SHARED ${SRC})

