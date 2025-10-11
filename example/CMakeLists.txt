# 定义一个函数创建 DLL
function(add_example EXAMPLENAME)
    file(GLOB SOURCES "${EXAMPLENAME}/*.cpp")
    #file(GLOB HEADERS "${EXAMPLENAME}/*.h")

    add_library(${EXAMPLENAME} SHARED ${SOURCES})

    # 去掉默认前缀，保持 DLL 名称
    set_target_properties(${EXAMPLENAME} PROPERTIES
        OUTPUT_NAME ${EXAMPLENAME}
        PREFIX ""
    )

    #target_include_directories(${EXAMPLENAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${EXAMPLENAME} leinterface)

    add_custom_command(TARGET ${EXAMPLENAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ">>>>>>>>>>>>>>>>>> Linked ${DLLEXAMPLENAMENAME}.dll <<<<<<<<<<<<<<<<<<"
    )
endfunction()

# 调用函数生成两个 DLL
add_example("example001")
add_example("example002")