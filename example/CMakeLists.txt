
function(add_example EXAMPLENAME)
    file(GLOB SOURCES "${EXAMPLENAME}/*.cpp")
    #file(GLOB HEADERS "${EXAMPLENAME}/*.h")

    add_library(${EXAMPLENAME} SHARED ${SOURCES})

    set_target_properties(${EXAMPLENAME} PROPERTIES
        OUTPUT_NAME ${EXAMPLENAME}
        PREFIX ""
    )

    #target_include_directories(${EXAMPLENAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${EXAMPLENAME} leinterface lecommon)

    add_custom_command(TARGET ${EXAMPLENAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${EXAMPLENAME}.dll <<<<<<<<<<<<<<<<<<"
    )
endfunction()


#add_example("SimpleTriangle")
#add_example("BasicTriangles")
#add_example("Bptpc16Texture")

file(GLOB EXAMPLE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/")

#exclude files(only pick folders)
set(EXAMPLE_DIRECTORIES "")
foreach(dir IN LISTS EXAMPLE_DIRS)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        list(APPEND EXAMPLE_DIRECTORIES ${dir})
    endif()
endforeach()

message(STATUS "Example CMakeLists.txt: Found example directories: ${EXAMPLE_DIRECTORIES}")

foreach(example_dir IN LISTS EXAMPLE_DIRECTORIES)
    add_example(${example_dir})
endforeach()