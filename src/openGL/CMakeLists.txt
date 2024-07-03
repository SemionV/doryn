cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-openGL)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c PROPERTIES LANGUAGE CXX )
add_library(dory-openGL ${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c)

target_include_directories(dory-openGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dory-openGL PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "openGL")

target_precompile_headers(dory-openGL PRIVATE dependencies.h)

target_link_libraries(dory-openGL base glfw)