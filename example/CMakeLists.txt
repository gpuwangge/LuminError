# 定义一个函数创建 DLL
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


add_example("SimpleTriangle")
add_example("BasicTriangles")