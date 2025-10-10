file(GLOB DIR "iexample")
add_library(leinterface INTERFACE)
target_include_directories(leinterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR})
